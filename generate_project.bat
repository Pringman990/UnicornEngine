@ECHO OFF
pushd %~dp0

mkdir Bin\
mkdir Temp\

call "Premake/premake5" --file=Source/premake5.lua vs2022 || @PAUSE

popd
@ECHO ON