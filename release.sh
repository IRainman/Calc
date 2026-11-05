#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: ./release.sh VERSION [COMMIT MESSAGE]

Examples:
  ./release.sh 2.10.5
  ./release.sh 2.10.5 "Prepare v2.10.5 release"

VERSION must have the form MAJOR.MINOR.PATCH.
EOF
    exit 1
}

if (( $# < 1 || $# > 2 )); then
    usage
fi

VERSION="$1"
TAG="v$VERSION"
MESSAGE="Prepare $TAG release"
if (( $# == 2 )); then
    MESSAGE="$2"
fi

if ! git rev-parse --show-toplevel >/dev/null 2>&1; then
    echo "ERROR: Run this script from inside the Calc Git repository." >&2
    exit 1
fi

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

BRANCH="$(git branch --show-current)"

if [[ -z "$BRANCH" ]]; then
    echo "ERROR: HEAD is detached." >&2
    exit 1
fi

if [[ ! "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "ERROR: Invalid version '$VERSION'; expected MAJOR.MINOR.PATCH." >&2
    exit 1
fi

if ! python3 - "$VERSION" <<'PY'
import re
import sys

parts = sys.argv[1].split(".")
if len(parts) != 3 or not all(re.fullmatch(r"[0-9]+", part) for part in parts):
    raise SystemExit("Invalid semantic version.")

if any(int(part) > 65535 for part in parts):
    raise SystemExit("Version components must be in the range 0..65535.")
PY
then
    exit 1
fi

RC_FILE="Calc/Calc.rc"
CMAKE_FILE="CMakeLists.txt"

for file in "$RC_FILE" "$CMAKE_FILE"; do
    if [[ ! -f "$file" ]]; then
        echo "ERROR: Required version file not found: $file" >&2
        exit 1
    fi
done

if [[ -n "$(git status --porcelain)" ]]; then
    echo "ERROR: The working tree is not clean." >&2
    echo "Commit, stash, or remove existing changes before preparing a release." >&2
    exit 1
fi

if git show-ref --verify --quiet "refs/tags/$TAG"; then
    echo "ERROR: Tag $TAG already exists locally." >&2
    exit 1
fi

if ! git remote get-url origin >/dev/null 2>&1; then
    echo "ERROR: Git remote 'origin' is not configured." >&2
    exit 1
fi

if ! REMOTE_TAGS="$(git ls-remote --tags origin "refs/tags/$TAG")"; then
    echo "ERROR: Unable to query tags from origin." >&2
    exit 1
fi

if [[ -n "$REMOTE_TAGS" ]]; then
    echo "ERROR: Tag $TAG already exists on origin." >&2
    exit 1
fi

BUILD="$(
    sed -n -E \
        's/^[[:space:]]*FILEVERSION[[:space:]]+[0-9]+,[0-9]+,[0-9]+,([0-9]+)[[:space:]]*$/\1/p' \
        "$RC_FILE"
)"

if [[ ! "$BUILD" =~ ^[0-9]+$ ]] || (( BUILD > 65535 )); then
    echo "ERROR: Could not read a valid fourth FILEVERSION component from $RC_FILE." >&2
    exit 1
fi

echo
echo "========================================"
echo "Calc release"
echo "========================================"
echo "Repository : $ROOT"
echo "Branch     : $BRANCH"
echo "Version    : $VERSION.$BUILD"
echo "Tag        : $TAG"
echo "Commit     : $MESSAGE"
echo

python3 - "$VERSION" "$BUILD" "$RC_FILE" "$CMAKE_FILE" <<'PY'
import codecs
import re
import sys
from pathlib import Path

version, build, rc_name, cmake_name = sys.argv[1:]
parts = version.split(".")

if len(parts) != 3 or any(not re.fullmatch(r"[0-9]+", p) for p in parts):
    raise SystemExit("Invalid version.")

if any(int(p) > 65535 for p in parts) or not re.fullmatch(r"[0-9]+", build):
    raise SystemExit("Version components must be in the range 0..65535.")

if int(build) > 65535:
    raise SystemExit("The FILEVERSION build component must be in the range 0..65535.")

full_version = f"{version}.{build}"
numeric_version = ",".join((*parts, build))


def read_utf8(path: Path):
    data = path.read_bytes()
    has_bom = data.startswith(codecs.BOM_UTF8)
    return data.decode("utf-8-sig"), has_bom


def write_utf8(path: Path, text: str, has_bom: bool):
    data = text.encode("utf-8")
    if has_bom:
        data = codecs.BOM_UTF8 + data
    path.write_bytes(data)


def replace_once(text: str, pattern: str, replacement: str, label: str) -> str:
    updated, count = re.subn(
        pattern,
        lambda match: match.group(1) + replacement + match.group(2),
        text,
        count=0,
        flags=re.MULTILINE,
    )
    if count != 1:
        raise SystemExit(f"Expected exactly one {label} field; found {count}.")
    return updated


rc_path = Path(rc_name)
cmake_path = Path(cmake_name)
rc, rc_bom = read_utf8(rc_path)
cmake, cmake_bom = read_utf8(cmake_path)

rc = replace_once(
    rc,
    r'^([ \t]*FILEVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$',
    numeric_version,
    "FILEVERSION",
)
rc = replace_once(
    rc,
    r'^([ \t]*PRODUCTVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$',
    numeric_version,
    "PRODUCTVERSION",
)
rc = replace_once(
    rc,
    r'(VALUE[ \t]+"FileVersion",[ \t]*")\d+\.\d+\.\d+\.\d+(")',
    full_version,
    "FileVersion",
)
rc = replace_once(
    rc,
    r'(VALUE[ \t]+"ProductVersion",[ \t]*")\d+\.\d+\.\d+\.\d+(")',
    full_version,
    "ProductVersion",
)
rc = replace_once(
    rc,
    r'(CTEXT[ \t]+"Calc[ \t]+)\d+\.\d+\.\d+\.\d+(")',
    full_version,
    "About-box version",
)
cmake = replace_once(
    cmake,
    r'^([ \t]*VERSION[ \t]+)\d+\.\d+\.\d+(?:\.\d+)?([ \t\r]*)$',
    full_version,
    "CMake project VERSION",
)

# Validate all replacements before modifying either file.
write_utf8(rc_path, rc, rc_bom)
write_utf8(cmake_path, cmake, cmake_bom)

print(f"Updated {rc_path}: {full_version}")
print(f"Updated {cmake_path}: {full_version}")
PY

git diff --check
git diff -- "$RC_FILE" "$CMAKE_FILE"

if git diff --quiet -- "$RC_FILE" "$CMAKE_FILE"; then
    echo "ERROR: The requested version is already present; no files changed." >&2
    exit 1
fi

echo
read -r -p "Commit and push release $TAG? [y/N] " ANSWER

case "$ANSWER" in
    y|Y|yes|YES)
        ;;
    *)
        git restore --worktree -- "$RC_FILE" "$CMAKE_FILE"
        echo "Release cancelled; version files restored."
        exit 1
        ;;
esac

git add -- "$RC_FILE" "$CMAKE_FILE"
git commit -m "$MESSAGE"

echo
echo "Pushing branch $BRANCH..."
git push origin "$BRANCH"

echo
echo "Creating annotated tag $TAG..."
git tag -a "$TAG" -m "$TAG"

echo "Pushing tag $TAG..."
if ! git push origin "$TAG"; then
    echo "ERROR: Tag push failed; deleting the local tag." >&2
    git tag -d "$TAG" >/dev/null 2>&1 || true
    exit 1
fi

echo
echo "Release $TAG was pushed successfully."
echo "GitHub Actions will build:"
echo "  - Windows x86 and x64 (MSVC)"
echo "  - Linux x86-64-v2 (GCC and Clang)"
echo "  - source archive with exact dependency revisions"
