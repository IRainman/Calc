#!/usr/bin/env sh

set -eu

usage() {
    echo "Usage: $0 VERSION [COMMIT MESSAGE]"
    echo
    echo "Example:"
    echo "  $0 2026.11.5"
    echo "  $0 2026.11.5 'Prepare v2026.11.5 release'"
    exit 1
}

if [ "$#" -lt 1 ]; then
    usage
fi

VERSION=$1
TAG="v$VERSION"
MESSAGE=$2
if [ "$#" -lt 2 ] || [ -z "$MESSAGE" ]; then
    MESSAGE="Prepare $TAG release"
fi

if ! printf '%s\n' "$VERSION" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "ERROR: Invalid version: $VERSION"
    echo "Expected format: X.Y.Z"
    exit 1
fi

ROOT=$(git rev-parse --show-toplevel 2>/dev/null) || {
    echo "ERROR: Current directory is not inside a Git repository."
    exit 1
}
cd "$ROOT"

BRANCH=$(git branch --show-current)
if [ -z "$BRANCH" ]; then
    echo "ERROR: HEAD is detached."
    exit 1
fi

RC_FILE="Calc/Calc.rc"
CMAKE_FILE="CMakeLists.txt"

for file in "$RC_FILE" "$CMAKE_FILE"; do
    if [ ! -f "$file" ]; then
        echo "ERROR: Required file was not found: $file"
        exit 1
    fi
done

if git rev-parse "$TAG" >/dev/null 2>&1; then
    echo "ERROR: Tag $TAG already exists locally."
    exit 1
fi

if git ls-remote --exit-code --tags origin "refs/tags/$TAG" >/dev/null 2>&1; then
    echo "ERROR: Tag $TAG already exists on origin."
    exit 1
fi

# Keep the fourth resource-version component as the build revision.
BUILD=$(sed -n -E \
    's/^[[:space:]]*FILEVERSION[[:space:]]+[0-9]+,[0-9]+,[0-9]+,([0-9]+)[[:space:]]*$/\1/p' \
    "$RC_FILE")

CURRENT_RC_VERSION=$(sed -n -E \
    's/^[[:space:]]*FILEVERSION[[:space:]]+([0-9]+),([0-9]+),([0-9]+),([0-9]+)[[:space:]]*$/\1.\2.\3.\4/p' \
    "$RC_FILE")

CURRENT_CMAKE_VERSION=$(sed -n -E \
    's/^[[:space:]]*VERSION[[:space:]]+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)[[:space:]]*$/\1/p' \
    "$CMAKE_FILE")

if [ -z "$BUILD" ] || [ -z "$CURRENT_RC_VERSION" ] || [ -z "$CURRENT_CMAKE_VERSION" ]; then
    echo "ERROR: Could not read the four-component version from Calc.rc or CMakeLists.txt."
    exit 1
fi

if [ "$CURRENT_RC_VERSION" != "$CURRENT_CMAKE_VERSION" ]; then
    echo "ERROR: Version mismatch before release:"
    echo "  Calc.rc:    $CURRENT_RC_VERSION"
    echo "  CMakeLists: $CURRENT_CMAKE_VERSION"
    exit 1
fi

FULL_VERSION="$VERSION.$BUILD"

echo
echo "========================================"
echo "Calc cross-platform release"
echo "========================================"
echo "Branch           : $BRANCH"
echo "Version          : $FULL_VERSION"
echo "Tag              : $TAG"
echo "Commit message   : $MESSAGE"
echo "Previous version : $CURRENT_RC_VERSION"
echo

VERSION="$VERSION" BUILD="$BUILD" FULL_VERSION="$FULL_VERSION" perl -0pi -e '
    my $full = $ENV{FULL_VERSION};
    my $numeric = $full;
    $numeric =~ s/\./,/g;

    my $file_count = () = /^[ \t]*FILEVERSION[ \t]+\d+,\d+,\d+,\d+[ \t]*$/mg;
    my $product_count = () = /^[ \t]*PRODUCTVERSION[ \t]+\d+,\d+,\d+,\d+[ \t]*$/mg;
    my $file_string_count = () = /VALUE[ \t]+"FileVersion",[ \t]*"\d+\.\d+\.\d+\.\d+"/g;
    my $product_string_count = () = /VALUE[ \t]+"ProductVersion",[ \t]*"\d+\.\d+\.\d+\.\d+"/g;
    my $about_count = () = /CTEXT[ \t]+"Calc[ \t]+\d+\.\d+\.\d+\.\d+"/g;

    die "Unexpected Calc.rc version-field count\n"
        if $file_count != 1 || $product_count != 1 ||
           $file_string_count != 1 || $product_string_count != 1 ||
           $about_count != 1;

    s{^([ \t]*FILEVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t]*)$}{$1$numeric$2}mg;
    s{^([ \t]*PRODUCTVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t]*)$}{$1$numeric$2}mg;
    s{VALUE[ \t]+"FileVersion",[ \t]*"\d+\.\d+\.\d+\.\d+"}{VALUE "FileVersion", "$full"}g;
    s{VALUE[ \t]+"ProductVersion",[ \t]*"\d+\.\d+\.\d+\.\d+"}{VALUE "ProductVersion", "$full"}g;
    s{CTEXT[ \t]+"Calc[ \t]+\d+\.\d+\.\d+\.\d+"}{CTEXT "Calc $full"}g;
' "$RC_FILE"

FULL_VERSION="$FULL_VERSION" perl -0pi -e '
    my $full = $ENV{FULL_VERSION};
    my $count = () = /^[ \t]*VERSION[ \t]+\d+\.\d+\.\d+\.\d+[ \t]*$/mg;
    die "Unexpected CMake project VERSION count: $count\n" if $count != 1;
    s{^([ \t]*VERSION[ \t]+)\d+\.\d+\.\d+\.\d+([ \t]*)$}{$1$full$2}m;
' "$CMAKE_FILE"

echo "Updated version fields:"
grep -E '^[[:space:]]*(FILEVERSION|PRODUCTVERSION)[[:space:]]|VALUE "FileVersion"|VALUE "ProductVersion"|CTEXT "Calc ' "$RC_FILE"
grep -E '^[[:space:]]*VERSION[[:space:]]+' "$CMAKE_FILE"

echo
git diff --check
git diff -- "$RC_FILE" "$CMAKE_FILE"

echo
printf "Continue with this release? [Y/N] "
read -r ANSWER

case "$ANSWER" in
    Y|y|YES|yes) ;;
    *)
        echo "Release cancelled."
        exit 1
        ;;
esac

git add -A
git commit -m "$MESSAGE"

echo
echo "Pushing branch $BRANCH..."
git push origin "$BRANCH"

echo
echo "Creating annotated tag $TAG..."
git tag -a "$TAG" -m "$TAG"

echo
echo "Pushing tag $TAG..."
if ! git push origin "$TAG"; then
    echo "ERROR: Tag push failed; removing local tag."
    git tag -d "$TAG" >/dev/null 2>&1 || true
    exit 1
fi

echo
echo "========================================"
echo "Release $TAG started successfully."
echo "========================================"
echo "GitHub Actions will:"
echo "  - build Windows x86 and x64 with MSVC"
echo "  - build Linux x86-64-v2 with GCC and Clang"
echo "  - package Windows executables without compression"
echo "  - package Windows PDB files separately"
echo "  - package Linux executables"
echo "  - package Calc and exact dependency source revisions"
echo "  - publish all assets to the GitHub Release"
