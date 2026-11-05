@echo off
setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" (
    echo Usage: %~nx0 VERSION [COMMIT MESSAGE]
    echo.
    echo Example:
    echo   %~nx0 2026.11.5
    echo   %~nx0 2026.11.5 "Prepare v2026.11.5 release"
    exit /b 1
)

set "VERSION=%~1"
set "TAG=v%VERSION%"
set "MESSAGE=%~2"
if "!MESSAGE!"=="" set "MESSAGE=Prepare %TAG% release"

echo %VERSION% | findstr /r /x "[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*" >nul
if errorlevel 1 (
    echo ERROR: Invalid version: %VERSION%
    echo Expected format: X.Y.Z
    exit /b 1
)

git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo ERROR: Current directory is not inside a Git repository.
    exit /b 1
)

for /f "delims=" %%B in ('git rev-parse --show-toplevel') do set "ROOT=%%B"
cd /d "!ROOT!"

for /f "delims=" %%B in ('git branch --show-current') do set "BRANCH=%%B"
if "!BRANCH!"=="" (
    echo ERROR: HEAD is detached.
    exit /b 1
)

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

git rev-parse "%TAG%" >nul 2>&1
if not errorlevel 1 (
    echo ERROR: Tag %TAG% already exists locally.
    exit /b 1
)

git ls-remote --exit-code --tags origin "refs/tags/%TAG%" >nul 2>&1
if not errorlevel 1 (
    echo ERROR: Tag %TAG% already exists on origin.
    exit /b 1
)

set "BUILD="
for /f "tokens=4 delims=, " %%A in ('findstr /r /c:"^[ ]*FILEVERSION[ ]" "%RC_FILE%"') do set "BUILD=%%A"
if "!BUILD!"=="" (
    echo ERROR: Could not read FILEVERSION from %RC_FILE%.
    exit /b 1
)

set "FULL_VERSION=%VERSION%.!BUILD!"

echo.
echo ========================================
echo Calc cross-platform release
echo ========================================
echo Branch           : !BRANCH!
echo Version          : !FULL_VERSION!
echo Tag              : %TAG%
echo Commit message   : !MESSAGE!
echo.

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ErrorActionPreference = 'Stop';" ^
  "$rcPath = [IO.Path]::GetFullPath('%RC_FILE%');" ^
  "$cmakePath = [IO.Path]::GetFullPath('%CMAKE_FILE%');" ^
  "$version = '%VERSION%'; $build = '%BUILD%'; $full = '%FULL_VERSION%';" ^
  "$rc = [IO.File]::ReadAllText($rcPath, [Text.Encoding]::UTF8);" ^
  "$cmake = [IO.File]::ReadAllText($cmakePath, [Text.Encoding]::UTF8);" ^
  "$rcMatch = [regex]::Match($rc, '(?m)^[ \t]*FILEVERSION[ \t]+([0-9]+),([0-9]+),([0-9]+),([0-9]+)[ \t]*$');" ^
  "$cmakeMatch = [regex]::Match($cmake, '(?m)^[ \t]*VERSION[ \t]+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)[ \t]*$');" ^
  "if (-not $rcMatch.Success -or -not $cmakeMatch.Success) { throw 'Could not read the existing four-part version.' };" ^
  "$currentRc = ($rcMatch.Groups[1].Value + '.' + $rcMatch.Groups[2].Value + '.' + $rcMatch.Groups[3].Value + '.' + $rcMatch.Groups[4].Value);" ^
  "if ($currentRc -ne $cmakeMatch.Groups[1].Value) { throw ('Version mismatch: Calc.rc=' + $currentRc + ', CMakeLists.txt=' + $cmakeMatch.Groups[1].Value) };" ^
  "function Replace-One([string]$text, [string]$pattern, [string]$replacement) { $regex = [regex]::new($pattern); if ($regex.Matches($text).Count -ne 1) { throw ('Expected exactly one match for: ' + $pattern) }; return $regex.Replace($text, $replacement, 1) };" ^
  "$numeric = $full.Replace('.', ',');" ^
  "$rc = Replace-One $rc '(?m)^([ \t]*FILEVERSION[ \t]+)[0-9]+,[0-9]+,[0-9]+,[0-9]+([ \t]*)$' ('$1' + $numeric + '$2');" ^
  "$rc = Replace-One $rc '(?m)^([ \t]*PRODUCTVERSION[ \t]+)[0-9]+,[0-9]+,[0-9]+,[0-9]+([ \t]*)$' ('$1' + $numeric + '$2');" ^
  "$rc = Replace-One $rc 'VALUE[ \t]+""FileVersion"",[ \t]*""[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+""' ('VALUE ""FileVersion"", ""' + $full + '""');" ^
  "$rc = Replace-One $rc 'VALUE[ \t]+""ProductVersion"",[ \t]*""[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+""' ('VALUE ""ProductVersion"", ""' + $full + '""');" ^
  "$rc = Replace-One $rc 'CTEXT[ \t]+""Calc[ \t]+[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+""' ('CTEXT ""Calc ' + $full + '""');" ^
  "$cmake = Replace-One $cmake '(?m)^([ \t]*VERSION[ \t]+)[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+([ \t]*)$' ('$1' + $full + '$2');" ^
  "[IO.File]::WriteAllText($rcPath, $rc, [Text.UTF8Encoding]::new($false));" ^
  "[IO.File]::WriteAllText($cmakePath, $cmake, [Text.UTF8Encoding]::new($false));" ^
  "Write-Host ('Updated version: ' + $full)"

if errorlevel 1 (
    echo ERROR: Failed to update the resource and CMake versions.
    exit /b 1
)

echo.
echo Updated resource version:
findstr /r /c:"^[ ]*FILEVERSION[ ]" /c:"^[ ]*PRODUCTVERSION[ ]" /c:"VALUE ""FileVersion""" /c:"VALUE ""ProductVersion""" /c:"CTEXT ""Calc " "%RC_FILE%"
echo.
echo Updated CMake version:
findstr /r /c:"^[ ]*VERSION[ ]" "%CMAKE_FILE%"

echo.
git diff --check
if errorlevel 1 (
    echo ERROR: git diff --check failed.
    exit /b 1
)
git diff -- "%RC_FILE%" "%CMAKE_FILE%"

echo.
choice /C YN /N /M "Continue with this release? [Y/N] "
if errorlevel 2 (
    echo Release cancelled.
    exit /b 1
)

git add -A
if errorlevel 1 (
    echo ERROR: git add failed.
    exit /b 1
)

git commit -m "!MESSAGE!"
if errorlevel 1 (
    echo ERROR: git commit failed.
    exit /b 1
)

echo.
echo Pushing branch !BRANCH!...
git push origin "!BRANCH!"
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

echo.
echo Pushing tag %TAG%...
git push origin "%TAG%"
if errorlevel 1 (
    echo ERROR: Tag push failed; removing local tag.
    git tag -d "%TAG%" >nul 2>&1
    exit /b 1
)

echo.
echo ========================================
echo Release %TAG% started successfully.
echo ========================================
echo GitHub Actions will:
echo   - build Windows x86 and x64 with MSVC
echo   - build Linux x86-64-v2 with GCC and Clang
echo   - package Windows executables without compression
echo   - package Windows PDB files separately
echo   - package Linux executables
echo   - package Calc and exact dependency source revisions
echo   - publish all assets to the GitHub Release
echo.

exit /b 0
