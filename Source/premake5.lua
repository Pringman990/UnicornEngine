include "../Premake/common.lua"

workspace "Unicorn"
    location "../"
    startproject "Launcher"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

include (dirs.engine)
include (dirs.graphics_engine)
include (dirs.game)
include (dirs.editor)
include (dirs.shaders)
include (dirs.externals)
include (dirs.launcher)