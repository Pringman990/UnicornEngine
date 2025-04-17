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

popd
@ECHO ON