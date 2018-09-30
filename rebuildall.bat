call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsMSBuildCmd.bat"

devenv Calc.sln /Rebuild "Release|Win32"

devenv Calc.sln /Rebuild "Release|x64"