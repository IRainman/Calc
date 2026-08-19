#!/usr/bin/env sh

set -eu

usage() {
    echo "Usage: $0 VERSION [COMMIT MESSAGE]"
    echo
    echo "Example:"
    echo "  $0 2.10.5"
    echo "  $0 2.10.5 'Prepare v2.10.5 release'"
    exit 1
}

if [ "$#" -lt 1 ]; then
    usage
fi

VERSION="$1"
TAG="v${VERSION}"

if [ "$#" -ge 2 ]; then
    MESSAGE="$2"
else
    MESSAGE="Prepare ${TAG} release"
fi

echo
echo "========================================"
echo "Calc release"
echo "========================================"
echo "Version : ${VERSION}"
echo "Tag     : ${TAG}"
echo "Message : ${MESSAGE}"
echo

# ----------------------------------------------------------------------
# Validate version: X.Y.Z
# ----------------------------------------------------------------------

case "${VERSION}" in
    *.*.*)
        ;;
    *)
        echo "ERROR: Invalid version: ${VERSION}"
        echo "Expected format: X.Y.Z"
        exit 1
        ;;
esac

case "${VERSION}" in
    ''|*[!0-9.]*)
        echo "ERROR: Invalid version: ${VERSION}"
        echo "Expected format: X.Y.Z"
        exit 1
        ;;
esac

MAJOR="${VERSION%%.*}"
REST="${VERSION#*.}"
MINOR="${REST%%.*}"
PATCH="${REST#*.}"

case "${MAJOR}" in
    ''|*[!0-9]*)
        echo "ERROR: Invalid major version."
        exit 1
        ;;
esac

case "${MINOR}" in
    ''|*[!0-9]*)
        echo "ERROR: Invalid minor version."
        exit 1
        ;;
esac

case "${PATCH}" in
    ''|*[!0-9]*)
        echo "ERROR: Invalid patch version."
        exit 1
        ;;
esac

# ----------------------------------------------------------------------
# Check Git repository.
# ----------------------------------------------------------------------

if ! git rev-parse --show-toplevel >/dev/null 2>&1; then
    echo "ERROR: Current directory is not a Git repository."
    exit 1
fi

BRANCH="$(git branch --show-current)"

if [ -z "${BRANCH}" ]; then
    echo "ERROR: HEAD is detached."
    exit 1
fi

echo "Current branch: ${BRANCH}"
echo

# ----------------------------------------------------------------------
# Locate Calc.rc.
# ----------------------------------------------------------------------

RC_FILE="Calc/Calc.rc"

if [ ! -f "${RC_FILE}" ]; then
    echo "ERROR: ${RC_FILE} was not found."
    echo "Run this script from the repository root."
    exit 1
fi

# ----------------------------------------------------------------------
# Check that the release tag does not already exist.
# ----------------------------------------------------------------------

if git rev-parse "${TAG}" >/dev/null 2>&1; then
    echo "ERROR: Tag ${TAG} already exists locally."
    exit 1
fi

if git ls-remote --exit-code --tags origin "refs/tags/${TAG}" >/dev/null 2>&1; then
    echo "ERROR: Tag ${TAG} already exists on origin."
    exit 1
fi

# ----------------------------------------------------------------------
# Read the current fourth version component from FILEVERSION.
# Example:
#     FILEVERSION 2,10,4,615
# ----------------------------------------------------------------------

BUILD="$(
    sed -n \
        -E 's/^[[:space:]]*FILEVERSION[[:space:]]+[0-9]+,[0-9]+,[0-9]+,([0-9]+)[[:space:]]*$/\1/p' \
        "${RC_FILE}"
)"

if [ -z "${BUILD}" ]; then
    echo "ERROR: Could not determine the fourth version component from ${RC_FILE}."
    exit 1
fi

echo "Resource version:"
echo "  Current build component: ${BUILD}"
echo "  New version: ${VERSION}.${BUILD}"
echo

# ----------------------------------------------------------------------
# Update Calc.rc.
#
# Updates:
#   FILEVERSION
#   PRODUCTVERSION
#   "FileVersion"
#   "ProductVersion"
#   About box "Calc X.Y.Z 2026"
#
# Perl is used because it handles the RC file reliably and preserves
# the rest of the file.
# ----------------------------------------------------------------------

VERSION="${VERSION}" BUILD="${BUILD}" perl -0pi -e '
    my $version = $ENV{VERSION};
    my $build   = $ENV{BUILD};

    my $numeric = $version;
    $numeric =~ s/\./,/g;
    $numeric .= "," . $build;

    my $full = $version . "." . $build;

    s{
        ^(\s*FILEVERSION\s+)\d+,\d+,\d+,\d+(\s*)$
    }{$1$numeric$2}mg;

    s{
        ^(\s*PRODUCTVERSION\s+)\d+,\d+,\d+,\d+(\s*)$
    }{$1$numeric$2}mg;

    s{
        VALUE\s+"FileVersion",\s*"\d+\.\d+\.\d+\.\d+"
    }{VALUE "FileVersion", "$full"}g;

    s{
        VALUE\s+"ProductVersion",\s*"\d+\.\d+\.\d+\.\d+"
    }{VALUE "ProductVersion", "$full"}g;

    s{
        CTEXT\s+"Calc\s+\d+\.\d+\.\d+\s+2026"
    }{CTEXT "Calc $version 2026"}g;
' "${RC_FILE}"

echo "Updated ${RC_FILE}:"

grep -E \
    '^[[:space:]]*(FILEVERSION|PRODUCTVERSION)[[:space:]]|VALUE "FileVersion"|VALUE "ProductVersion"|CTEXT "Calc ' \
    "${RC_FILE}"

echo

# ----------------------------------------------------------------------
# Show resulting change.
# ----------------------------------------------------------------------

git diff -- "${RC_FILE}"

echo
printf "Continue with this release? [Y/N] "
read -r ANSWER

case "${ANSWER}" in
    Y|y|YES|yes)
        ;;
    *)
        echo "Release cancelled."
        exit 1
        ;;
esac

# ----------------------------------------------------------------------
# Commit all changes.
# ----------------------------------------------------------------------

git add -A

git commit -m "${MESSAGE}"

# ----------------------------------------------------------------------
# Push branch.
# ----------------------------------------------------------------------

echo
echo "Pushing branch ${BRANCH}..."
git push origin "${BRANCH}"

# ----------------------------------------------------------------------
# Create annotated release tag.
# ----------------------------------------------------------------------

echo
echo "Creating tag ${TAG}..."
git tag -a "${TAG}" -m "${TAG}"

# ----------------------------------------------------------------------
# Push tag.
# ----------------------------------------------------------------------

echo
echo "Pushing tag ${TAG}..."

if ! git push origin "${TAG}"; then
    echo "ERROR: Tag push failed."
    echo "Removing local tag..."
    git tag -d "${TAG}" >/dev/null 2>&1 || true
    exit 1
fi

echo
echo "========================================"
echo "Release ${TAG} started successfully."
echo "========================================"
echo
echo "GitHub Actions will now:"
echo "  - build Windows x86"
echo "  - build Windows x64"
echo "  - package Calc.exe"
echo "  - package Calc.pdb"
echo "  - package the source tree"
echo "  - create the GitHub Release"
echo