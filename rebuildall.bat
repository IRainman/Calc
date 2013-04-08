call "%VS110COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
"%VS110COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|Win32"

call "%VS110COMNTOOLS%\..\..\VC\bin\amd64\vcvars64.bat"
"%VS110COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|x64"