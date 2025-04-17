@ECHO OFF
pushd %~dp0..

mkdir Binaries\Win64
mkdir Binaries\Shaders

mkdir Intermediate\
mkdir Intermediate\ProjectFiles
mkdir Intermediate\Temp
mkdir Intermediate\CompileCommands

:: Generate project files for VS2022
call "Scripts/Premake/premake5" --file=Source/premake5.lua vs2022 || @PAUSE

:: Generate compile_commands.json
:: call "Scripts/Premake/premake5" --file=Source/premake5.lua export-compile-commands || @PAUSE
:: 
:: :: Move it where you want
:: move compile_commands.json Intermediate/CompileCommands/compile_commands_Debug.json
:: 
:: :: Move all JSON files to your own folder
:: move /Y compile_commands\*.json Intermediate/CompileCommands/
:: 
:: :: Delete the now-empty original folder
:: rd /S /Q compile_commands

popd
@Pause
@ECHO ON