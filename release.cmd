@echo off
setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" (
    echo Usage: %~nx0 VERSION [COMMIT MESSAGE]
    echo.
    echo Example:
    echo   %~nx0 2.10.5
    echo   %~nx0 2.10.5 "Prepare v2.10.5 release"
    exit /b 1
)

set "VERSION=%~1"
set "TAG=v%VERSION%"
set "MESSAGE=%~2"

if "%MESSAGE%"=="" (
    set "MESSAGE=Prepare %TAG% release"
)

echo.
echo ========================================
echo Calc release
echo ========================================
echo Version : %VERSION%
echo Tag     : %TAG%
echo Message : %MESSAGE%
echo.

rem ----------------------------------------------------------------------
rem Validate version: X.Y.Z
rem ----------------------------------------------------------------------

echo %VERSION% | findstr /r /x "[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*" >nul
if errorlevel 1 (
    echo ERROR: Invalid version: %VERSION%
    echo Expected format: X.Y.Z
    exit /b 1
)

for /f "tokens=1-3 delims=." %%A in ("%VERSION%") do (
    set "MAJOR=%%A"
    set "MINOR=%%B"
    set "PATCH=%%C"
)

rem ----------------------------------------------------------------------
rem Check Git repository.
rem ----------------------------------------------------------------------

git rev-parse --show-toplevel >nul 2>&1
if errorlevel 1 (
    echo ERROR: Current directory is not a Git repository.
    exit /b 1
)

for /f "delims=" %%B in ('git branch --show-current') do set "BRANCH=%%B"

if "!BRANCH!"=="" (
    echo ERROR: HEAD is detached.
    exit /b 1
)

echo Current branch: !BRANCH!
echo.

rem ----------------------------------------------------------------------
rem Locate Calc.rc.
rem ----------------------------------------------------------------------

if not exist "Calc\Calc.rc" (
    echo ERROR: Calc\Calc.rc was not found.
    echo Run this script from the repository root.
    exit /b 1
)

set "RC_FILE=Calc\Calc.rc"

rem ----------------------------------------------------------------------
rem Check that the release tag does not already exist.
rem ----------------------------------------------------------------------

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

rem ----------------------------------------------------------------------
rem Read the current fourth version component from FILEVERSION.
rem Example:
rem     FILEVERSION 2,10,4,615
rem ----------------------------------------------------------------------

set "BUILD="
for /f "tokens=4 delims=, " %%A in ('findstr /r /c:"^[ ]*FILEVERSION[ ]" "%RC_FILE%"') do (
    set "BUILD=%%A"
)

if "!BUILD!"=="" (
    echo ERROR: Could not determine the fourth version component from %RC_FILE%.
    exit /b 1
)

echo Resource version:
echo   Current build component: !BUILD!
echo   New version: %VERSION%.!BUILD!
echo.

rem ----------------------------------------------------------------------
rem Update Calc.rc.
rem
rem Updates:
rem   FILEVERSION
rem   PRODUCTVERSION
rem   "FileVersion"
rem   "ProductVersion"
rem   About box "Calc X.Y.Z 2026"
rem ----------------------------------------------------------------------

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command ^
    "$path = [System.IO.Path]::GetFullPath('%RC_FILE%');" ^
    "$text = [System.IO.File]::ReadAllText($path, [System.Text.UTF8Encoding]::new($false));" ^
    "$build = '%BUILD%';" ^
    "$version = '%VERSION%';" ^
    "$numeric = $version.Replace('.', ',') + ',' + $build;" ^
    "$full = $version + '.' + $build;" ^
    "$text = [regex]::Replace($text, '(?m)^(\s*FILEVERSION\s+)\d+,\d+,\d+,\d+(\s*)$', '${1}' + $numeric + '${2}');" ^
    "$text = [regex]::Replace($text, '(?m)^(\s*PRODUCTVERSION\s+)\d+,\d+,\d+,\d+(\s*)$', '${1}' + $numeric + '${2}');" ^
    "$text = [regex]::Replace($text, 'VALUE\s+""FileVersion"",\s*""\d+\.\d+\.\d+\.\d+""', 'VALUE ""FileVersion"", ""' + $full + '""');" ^
    "$text = [regex]::Replace($text, 'VALUE\s+""ProductVersion"",\s*""\d+\.\d+\.\d+\.\d+""', 'VALUE ""ProductVersion"", ""' + $full + '""');" ^
    "$text = [regex]::Replace($text, 'CTEXT\s+""Calc\s+\d+\.\d+\.\d+\s+2026""', 'CTEXT ""Calc ' + $version + ' 2026""');" ^
    "[System.IO.File]::WriteAllText($path, $text, [System.Text.UTF8Encoding]::new($false))"

if errorlevel 1 (
    echo ERROR: Failed to update %RC_FILE%.
    exit /b 1
)

echo Updated %RC_FILE%:
findstr /r /c:"^[ ]*FILEVERSION[ ]" /c:"^[ ]*PRODUCTVERSION[ ]" /c:"VALUE ""FileVersion""" /c:"VALUE ""ProductVersion""" /c:"CTEXT ""Calc " "%RC_FILE%"

echo.

rem ----------------------------------------------------------------------
rem Check resulting Git diff.
rem ----------------------------------------------------------------------

git diff -- "%RC_FILE%"

echo.
choice /C YN /N /M "Continue with this release? [Y/N] "
if errorlevel 2 (
    echo Release cancelled.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Commit all changes.
rem ----------------------------------------------------------------------

git add -A
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
rem Push branch.
rem ----------------------------------------------------------------------

echo.
echo Pushing branch !BRANCH!...
git push origin "!BRANCH!"
if errorlevel 1 (
    echo ERROR: Branch push failed.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Create annotated release tag.
rem ----------------------------------------------------------------------

echo.
echo Creating tag %TAG%...
git tag -a "%TAG%" -m "%TAG%"
if errorlevel 1 (
    echo ERROR: Tag creation failed.
    exit /b 1
)

rem ----------------------------------------------------------------------
rem Push tag.
rem ----------------------------------------------------------------------

echo.
echo Pushing tag %TAG%...
git push origin "%TAG%"
if errorlevel 1 (
    echo ERROR: Tag push failed.
    echo Removing local tag because the remote push failed...
    git tag -d "%TAG%" >nul 2>&1
    exit /b 1
)

echo.
echo ========================================
echo Release %TAG% started successfully.
echo ========================================
echo.
echo GitHub Actions will now:
echo   - build Windows x86
echo   - build Windows x64
echo   - package Calc.exe
echo   - package Calc.pdb
echo   - package the source tree
echo   - create the GitHub Release
echo.

exit /b 0