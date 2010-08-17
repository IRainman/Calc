call "%VS100COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
"%VS100COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|Win32"

call "%VS100COMNTOOLS%\..\..\VC\bin\amd64\vcvars64.bat"
"%VS100COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|x64"