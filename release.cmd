@echo off
setlocal EnableExtensions

if not "%~2"=="" (
    echo Usage: %~nx0 [COMMIT MESSAGE]
    echo.
    echo The release version is generated as YYYY.MM.DD.N.
    echo N starts at 0 on a new date and increments for each release on that date.
    exit /b 1
)

set "ROOT="
set "BRANCH="
set "DIRTY="
set "REMOTE_TAG="
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
rem Generate YYYY.MM.DD.N from the system date and current FILEVERSION.
rem Reset N on a new date; increment N when releasing again on the same day.
rem ----------------------------------------------------------------------

set "VERSION_FILE=%TEMP%\Calc-release-version-%RANDOM%.txt"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
    "$ErrorActionPreference = 'Stop';" ^
    "$rc = [System.IO.File]::ReadAllText((Join-Path (Get-Location) 'Calc\Calc.rc'));" ^
    "$matches = [regex]::Matches($rc, '(?m)^[ \t]*FILEVERSION[ \t]+(\d+),(\d+),(\d+),(\d+)[ \t\r]*$');" ^
    "if ($matches.Count -ne 1) { throw ('Expected exactly one FILEVERSION field; found ' + $matches.Count + '.') };" ^
    "$previous = $matches[0].Groups;" ^
    "$previousParts = @([int]$previous[1].Value, [int]$previous[2].Value, [int]$previous[3].Value, [int]$previous[4].Value);" ^
    "if ($previousParts | Where-Object { $_ -gt 65535 }) { throw 'Existing FILEVERSION components must be in the range 0..65535.' };" ^
    "$today = [DateTime]::Now.ToString('yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture);" ^
    "$previousDate = '{0:D4}.{1:D2}.{2:D2}' -f $previousParts[0], $previousParts[1], $previousParts[2];" ^
    "$sequence = 0;" ^
    "if ($previousDate -ceq $today) { $sequence = $previousParts[3] + 1 };" ^
    "if ($sequence -gt 65535) { throw 'The daily release counter exceeded the Windows version limit (65535).' };" ^
    "[Console]::WriteLine($today + '.' + $sequence)" > "%VERSION_FILE%"

if errorlevel 1 (
    del "%VERSION_FILE%" >nul 2>&1
    echo ERROR: Unable to generate the date-based release version.
    exit /b 1
)

set /p VERSION=<"%VERSION_FILE%"
del "%VERSION_FILE%" >nul 2>&1

if not defined VERSION (
    echo ERROR: The generated release version is empty.
    exit /b 1
)

set "TAG=v%VERSION%"

if "%MESSAGE%"=="" (
    set "MESSAGE=Prepare %TAG% release"
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
rem The fourth version component is the per-day release counter.
rem ----------------------------------------------------------------------

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
    "$ErrorActionPreference = 'Stop';" ^
    "$version = $env:VERSION;" ^
    "$parts = $version.Split('.');" ^
    "if ($parts.Count -ne 4 -or $version -notmatch '^[0-9]{4}\.[0-9]{2}\.[0-9]{2}\.[0-9]+$') { throw 'Invalid date-based version.' };" ^
    "$parsedDate = [DateTime]::MinValue;" ^
    "if (-not [DateTime]::TryParseExact(($parts[0] + '.' + $parts[1] + '.' + $parts[2]), 'yyyy.MM.dd', [Globalization.CultureInfo]::InvariantCulture, [Globalization.DateTimeStyles]::None, [ref]$parsedDate)) { throw 'Invalid date in version.' };" ^
    "$full = $version;" ^
    "if ([int64]$parts[3] -gt 65535) { throw 'Daily release counter must be in the range 0..65535.' };" ^
    "$numeric = '{0},{1},{2},{3}' -f [int]$parts[0], [int]$parts[1], [int]$parts[2], [int]$parts[3];" ^
    "$utf8 = [System.Text.UTF8Encoding]::new($false, $true);" ^
    "function Replace-One([byte[]]$data, [string]$pattern, [string]$replacement, [string]$label) {" ^
    "  $offset = 0;" ^
    "  if ($data.Length -ge 3 -and $data[0] -eq 0xEF -and $data[1] -eq 0xBB -and $data[2] -eq 0xBF) { $offset = 3 };" ^
    "  $text = $utf8.GetString($data, $offset, $data.Length - $offset);" ^
    "  $matches = [regex]::Matches($text, $pattern);" ^
    "  if ($matches.Count -ne 1) { throw ('Expected exactly one ' + $label + ' field; found ' + $matches.Count + '.') };" ^
    "  $match = $matches[0];" ^
    "  $replacementText = $match.Groups[1].Value + $replacement + $match.Groups[2].Value;" ^
    "  $start = $offset + $utf8.GetByteCount($text.Substring(0, $match.Index));" ^
    "  $oldLength = $utf8.GetByteCount($match.Value);" ^
    "  $replacementBytes = $utf8.GetBytes($replacementText);" ^
    "  $result = New-Object byte[] ($data.Length - $oldLength + $replacementBytes.Length);" ^
    "  [Array]::Copy($data, 0, $result, 0, $start);" ^
    "  [Array]::Copy($replacementBytes, 0, $result, $start, $replacementBytes.Length);" ^
    "  $tailStart = $start + $oldLength;" ^
    "  $tailLength = $data.Length - $tailStart;" ^
    "  [Array]::Copy($data, $tailStart, $result, $start + $replacementBytes.Length, $tailLength);" ^
    "  return ,([byte[]]$result);" ^
    "};" ^
    "$rcPath = Join-Path (Get-Location) 'Calc\Calc.rc';" ^
    "$cmakePath = Join-Path (Get-Location) 'CMakeLists.txt';" ^
    "$rc = [System.IO.File]::ReadAllBytes($rcPath);" ^
    "$cmake = [System.IO.File]::ReadAllBytes($cmakePath);" ^
    "$rc = Replace-One $rc '(?m)^([ \t]*FILEVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$' $numeric 'FILEVERSION';" ^
    "$rc = Replace-One $rc '(?m)^([ \t]*PRODUCTVERSION[ \t]+)\d+,\d+,\d+,\d+([ \t\r]*)$' $numeric 'PRODUCTVERSION';" ^
    "$rc = Replace-One $rc '(VALUE[ \t]+\x22FileVersion\x22,[ \t]*\x22)\d+\.\d+\.\d+\.\d+(\x22)' $full 'FileVersion';" ^
    "$rc = Replace-One $rc '(VALUE[ \t]+\x22ProductVersion\x22,[ \t]*\x22)\d+\.\d+\.\d+\.\d+(\x22)' $full 'ProductVersion';" ^
    "$rc = Replace-One $rc '(CTEXT[ \t]+\x22Calc[ \t]+)\d+\.\d+\.\d+\.\d+(\x22)' $full 'About-box version';" ^
    "$cmake = Replace-One $cmake '(?m)^([ \t]*VERSION[ \t]+)\d+\.\d+\.\d+(?:\.\d+)?([ \t\r]*)$' $full 'CMake project VERSION';" ^
    "[System.IO.File]::WriteAllBytes($rcPath, $rc);" ^
    "[System.IO.File]::WriteAllBytes($cmakePath, $cmake);" ^
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
