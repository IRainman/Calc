call "%VS140COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
"%VS140COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|Win32"

call "%VS140COMNTOOLS%\..\..\VC\bin\amd64\vcvars64.bat"
"%VS140COMNTOOLS%..\ide\devenv" Calc.sln /Rebuild "Release|x64"