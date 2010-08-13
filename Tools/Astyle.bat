SETLOCAL ENABLEDELAYEDEXPANSION
set START_DIR=%~d0%~p0%~nx0
set START_DIR=!START_DIR:%~0=!
echo %START_DIR%

cd "%~d0%~p0"

astyle --indent=tab=4 --brackets=break --indent-classes --indent-switches --indent-preprocessor --break-closing-brackets --pad-oper --pad-header --unpad-paren --convert-tabs --fill-empty-lines --max-instatement-indent=79 ..\Calc\*.cpp ..\Calc\*.h

cd %START_DIR%