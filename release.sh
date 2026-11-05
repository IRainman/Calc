#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: ./release.sh [COMMIT MESSAGE]

The release version is generated from the system date as YYYY.MM.DD.0.
EOF
    exit 1
}

if (( $# > 1 )); then
    usage
fi

DATE_VERSION="$(LC_ALL=C date +%Y.%m.%d)" || {
    echo "ERROR: Unable to read the system date." >&2
    exit 1
}

if [[ ! "$DATE_VERSION" =~ ^[0-9]{4}\.[0-9]{2}\.[0-9]{2}$ ]]; then
    echo "ERROR: Unexpected system date format: $DATE_VERSION" >&2
    exit 1
fi

VERSION="${DATE_VERSION}.0"
TAG="v$VERSION"
MESSAGE="${1:-Prepare $TAG release}"

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

echo
echo "========================================"
echo "Calc release"
echo "========================================"
echo "Repository : $ROOT"
echo "Branch     : $BRANCH"
echo "Version    : $VERSION"
echo "Tag        : $TAG"
echo "Commit     : $MESSAGE"
echo

python3 - "$VERSION" "$RC_FILE" "$CMAKE_FILE" <<'PY'
import codecs
import re
import sys
from datetime import datetime
from pathlib import Path

version, rc_name, cmake_name = sys.argv[1:]

parts = version.split(".")
if len(parts) != 4 or parts[3] != "0":
    raise SystemExit("Expected a four-component YYYY.MM.DD.0 version.")

if any(not re.fullmatch(r"[0-9]+", part) for part in parts):
    raise SystemExit("Invalid version.")

try:
    datetime.strptime(".".join(parts[:3]), "%Y.%m.%d")
except ValueError as error:
    raise SystemExit(f"Invalid date in version {version}: {error}") from error

if any(int(part) > 65535 for part in parts):
    raise SystemExit("Version components must be in the range 0..65535.")

full_version = version
numeric_version = ",".join(str(int(part)) for part in parts)


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
    echo "ERROR: The current version is already present; no files changed." >&2
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
