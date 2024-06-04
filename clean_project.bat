@echo off
:: Main Directory
@RD /S /Q "./.vs"
@RD /S /Q "./.vscode"
@RD /S /Q "./Temp"
@RD /S /Q "./JSON"
@DEL "./*.sln" /Q

:: Bin
cd "./Bin"
@RD /S /Q "./Shaders"
@DEL "./*.dll" /Q
@DEL "./*.exe" /Q
@DEL "./*.pdb" /Q
@DEL "./imgui.ini" /Q
@DEL "./gaia_profile.prof" /Q
cd ".."

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