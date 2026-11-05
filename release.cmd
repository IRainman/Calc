@echo off
setlocal EnableExtensions

if not "%~2"=="" (
    echo Usage: %~nx0 [COMMIT MESSAGE]
    echo.
    echo The release version is generated from the system date as YYYY.MM.DD.0.
    exit /b 1
)

set "ROOT="
set "BRANCH="
set "DIRTY="
set "REMOTE_TAG="
set "DATE_VERSION="
set "VERSION="
set "TAG="
set "MESSAGE=%~1"

rem ----------------------------------------------------------------------
rem Locate the repository root and verify the current branch.
rem ----------------------------------------------------------------------

for /f "delims=" %%R in ('git rev-parse --show-toplevel 2^>nul') do set "ROOT=%%R"

if not defined ROOT (
    echo ERROR: Current directory is not inside a Git repository.
    exit /b 1
)

cd /d "%ROOT%"

for /f "delims=" %%B in ('git branch --show-current 2^>nul') do set "BRANCH=%%B"

if not defined BRANCH (
    echo ERROR: HEAD is detached.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Generate the date-based version from the local system clock.
rem ----------------------------------------------------------------------

for /f "delims=" %%D in ('powershell.exe -NoProfile -Command "([DateTime]::Now).ToString('yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture)" 2^>nul') do set "DATE_VERSION=%%D"

if not defined DATE_VERSION (
    echo ERROR: Unable to read the system date.
    exit /b 1
)

powershell.exe -NoProfile -Command ^
    "if ($env:DATE_VERSION -match '^[0-9]{4}\.[0-9]{2}\.[0-9]{2}$') { exit 0 } else { exit 1 }"

if errorlevel 1 (
    echo ERROR: Unexpected system date format: %DATE_VERSION%
    exit /b 1
)

set "VERSION=%DATE_VERSION%.0"
set "TAG=v%VERSION%"

if "%MESSAGE%"=="" (
    set "MESSAGE=Prepare %TAG% release"
)

rem ----------------------------------------------------------------------
rem Validate the required version files.
rem ----------------------------------------------------------------------

set "RC_FILE=Calc\Calc.rc"
set "CMAKE_FILE=CMakeLists.txt"

if not exist "%RC_FILE%" (
    echo ERROR: Required file was not found: %RC_FILE%
    exit /b 1
)

if not exist "%CMAKE_FILE%" (
    echo ERROR: Required file was not found: %CMAKE_FILE%
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Require a clean working tree so unrelated changes are never committed.
rem ----------------------------------------------------------------------

set "STATUS_FILE=%TEMP%\Calc-release-status-%RANDOM%.txt"
git status --porcelain > "%STATUS_FILE%"

if errorlevel 1 (
    del "%STATUS_FILE%" >nul 2>&1
    echo ERROR: Unable to inspect the Git working tree.
    exit /b 1
)

for %%F in ("%STATUS_FILE%") do if %%~zF GTR 0 set "DIRTY=1"
del "%STATUS_FILE%" >nul 2>&1

if defined DIRTY (
    echo ERROR: The working tree is not clean.
    echo Commit, stash, or remove existing changes before preparing a release.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Check local and remote release tags.
rem ----------------------------------------------------------------------

git show-ref --verify --quiet "refs/tags/%TAG%"
if not errorlevel 1 (
    echo ERROR: Tag %TAG% already exists locally.
    exit /b 1
)

git remote get-url origin >nul 2>&1
if errorlevel 1 (
    echo ERROR: Git remote "origin" is not configured.
    exit /b 1
)

set "REMOTE_TAG_FILE=%TEMP%\Calc-release-tags-%RANDOM%.txt"
git ls-remote --tags origin "refs/tags/%TAG%" > "%REMOTE_TAG_FILE%"

if errorlevel 1 (
    del "%REMOTE_TAG_FILE%" >nul 2>&1
    echo ERROR: Unable to query tags from origin.
    exit /b 1
)

for %%F in ("%REMOTE_TAG_FILE%") do if %%~zF GTR 0 set "REMOTE_TAG=1"
del "%REMOTE_TAG_FILE%" >nul 2>&1

if defined REMOTE_TAG (
    echo ERROR: Tag %TAG% already exists on origin.
    exit /b 1
)

echo.
echo ========================================
echo Calc release
echo ========================================
echo Repository : %ROOT%
echo Branch     : %BRANCH%
echo Version    : %VERSION%
echo Tag        : %TAG%
echo Commit     : %MESSAGE%
echo.

rem ----------------------------------------------------------------------
rem Update both the Windows resource version and CMake project version.
rem The fourth version component is always zero.
rem ----------------------------------------------------------------------

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
    "$ErrorActionPreference = 'Stop';" ^
    "$version = $env:VERSION;" ^
    "$parts = $version.Split('.');" ^
    "if ($parts.Count -ne 4 -or $parts[3] -ne '0' -or $version -notmatch '^[0-9]{4}\.[0-9]{2}\.[0-9]{2}\.0$') { throw 'Invalid date-based version.' };" ^
    "$parsedDate = [DateTime]::MinValue;" ^
    "if (-not [DateTime]::TryParseExact(($parts[0] + '.' + $parts[1] + '.' + $parts[2]), 'yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture, [Globalization.DateTimeStyles]::None, [ref]$parsedDate)) { throw 'Invalid date in version.' };" ^
    "$full = $version;" ^
    "$numeric = '{0},{1},{2},0' -f [int]$parts[0], [int]$parts[1], [int]$parts[2];" ^
    "function Replace-One([string]$text, [string]$pattern, [string]$replacement, [string]$label) {" ^
    "  $count = [regex]::Matches($text, $pattern).Count;" ^
    "  if ($count -ne 1) { throw ('Expected exactly one ' + $label + ' field; found ' + $count + '.') };" ^
    "  return [regex]::Replace($text, $pattern, $replacement, 1);" ^
    "};" ^
    "$rcPath = Join-Path (Get-Location) 'Calc\Calc.rc';" ^
    "$cmakePath = Join-Path (Get-Location) 'CMakeLists.txt';" ^
    "$rc = [System.IO.File]::ReadAllText($rcPath);" ^
    "$cmake = [System.IO.File]::ReadAllText($cmakePath);" ^
    "$rc = Replace-One $rc '(?m)^([ \t]*FILEVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$' ('$1' + $numeric + '$2') 'FILEVERSION';" ^
    "$rc = Replace-One $rc '(?m)^([ \t]*PRODUCTVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$' ('$1' + $numeric + '$2') 'PRODUCTVERSION';" ^
    "$rc = Replace-One $rc '(VALUE[ \t]+\x22FileVersion\x22,[ \t]*\x22)\d+\.\d+\.\d+\.\d+(\x22)' ('$1' + $full + '$2') 'FileVersion';" ^
    "$rc = Replace-One $rc '(VALUE[ \t]+\x22ProductVersion\x22,[ \t]*\x22)\d+\.\d+\.\d+\.\d+(\x22)' ('$1' + $full + '$2') 'ProductVersion';" ^
    "$rc = Replace-One $rc '(CTEXT[ \t]+\x22Calc[ \t]+)\d+\.\d+\.\d+\.\d+(\x22)' ('$1' + $full + '$2') 'About-box version';" ^
    "$cmake = Replace-One $cmake '(?m)^([ \t]*VERSION[ \t]+)\d+\.\d+\.\d+(\.\d+)?([ \t\r]*)$' ('$1' + $full + '$3') 'CMake project VERSION';" ^
    "[System.IO.File]::WriteAllText($rcPath, $rc, [System.Text.UTF8Encoding]::new($true));" ^
    "[System.IO.File]::WriteAllText($cmakePath, $cmake, [System.Text.UTF8Encoding]::new($false));" ^
    "Write-Host ('Updated Calc.rc and CMakeLists.txt to ' + $full);"

if errorlevel 1 (
    echo ERROR: Failed to update version files.
    exit /b 1
)

git diff --check
if errorlevel 1 (
    echo ERROR: git diff --check failed.
    git restore --worktree -- "%RC_FILE%" "%CMAKE_FILE%"
    exit /b 1
)

git diff -- "%RC_FILE%" "%CMAKE_FILE%"

git diff --quiet -- "%RC_FILE%" "%CMAKE_FILE%"
if not errorlevel 1 (
    echo ERROR: The current version is already present; no files changed.
    exit /b 1
)

echo.
choice /C YN /N /M "Commit and push release %TAG%? [Y/N] "
if errorlevel 2 (
    git restore --worktree -- "%RC_FILE%" "%CMAKE_FILE%"
    echo Release cancelled; version files restored.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Commit only the two version files.
rem ----------------------------------------------------------------------

git add -- "%RC_FILE%" "%CMAKE_FILE%"
if errorlevel 1 (
    echo ERROR: git add failed.
    exit /b 1
)

git commit -m "%MESSAGE%"
if errorlevel 1 (
    echo ERROR: git commit failed.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Push the release commit and then the annotated tag.
rem ----------------------------------------------------------------------

echo.
echo Pushing branch %BRANCH%...
git push origin "%BRANCH%"
if errorlevel 1 (
    echo ERROR: Branch push failed.
    exit /b 1
)

echo.
echo Creating annotated tag %TAG%...
git tag -a "%TAG%" -m "%TAG%"
if errorlevel 1 (
    echo ERROR: Tag creation failed.
    exit /b 1
)

echo Pushing tag %TAG%...
git push origin "%TAG%"
if errorlevel 1 (
    echo ERROR: Tag push failed; deleting the local tag.
    git tag -d "%TAG%" >nul 2>&1
    exit /b 1
)

echo.
echo Release %TAG% was pushed successfully.
echo GitHub Actions will build:
echo   - Windows x86 and x64 (MSVC)
echo   - Linux x86-64-v2 (GCC and Clang)
echo   - source archive with exact dependency revisions
echo.

exit /b 0
