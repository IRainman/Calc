call "c:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsMSBuildCmd.bat"

start devenv Calc.sln /Rebuild "Release|Win32"

start devenv Calc.sln /Rebuild "Release|x64"