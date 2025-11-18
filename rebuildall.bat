call "c:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsMSBuildCmd.bat"

devenv Calc.sln /Rebuild "Release|Win32"

devenv Calc.sln /Rebuild "Release|x64"
