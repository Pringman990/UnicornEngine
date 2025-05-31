@echo off
pushd %~dp0..

:: Main Directory
@RD /S /Q "./.vs"
@RD /S /Q "./.vscode"
@RD /S /Q "./Intermediate"
@DEL "./*.sln" /Q

:: Bin
@RD /S /Q "./Binaries/Win64"
@RD /S /Q "./Binaries/Shaders"
@RD /S /Q "./Binaries/Logs"


:: VS Project Files
@DEL "./*vcxproj*" /S /Q

:: Intel VTune
@DEL "./debug.log" /Q
@RD /S /Q "./DawnCache"
@RD /S /Q "./GPUCache"

set "folderPaths=Local"
for %%d in (%folderPaths%) do (
    rd /s /q "%%d"
)

popd