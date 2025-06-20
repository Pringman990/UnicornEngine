print("Including Source/")

include "../Scripts/Premake/common.lua"

solution "Unicorn Engine"
    location "../"
    startproject "EntryPoint"
    
    configurations {
        "MemoryDebug",
        "MemoryDebug_Editor",
        "Debug",
        "Debug_Editor",
        "Retail",
		"Tests"
    }

	platforms{ 
		"x64-windows",
		"x64-sdl",
	}

	architecture "x86_64"

	filter ("platforms:x64-windows")
		defines {"_Win32", "_Win64"}

	filter ("platforms:x64-sdl")
		defines {"_SDL"}

  	filter ("configurations:MemoryDebug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="MemoryDebug"', "_MEMORY_DEBUG", "_DEBUG"}
		flags {"FatalWarnings"}

	filter ("configurations:MemoryDebug_Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="MemoryDebug_Editor"', "_MEMORY_DEBUG", "_DEBUG", "_EDITOR"}
		flags {"FatalWarnings"}

    filter ("configurations:Debug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="Debug"', "_DEBUG"}
		flags {"FatalWarnings"}

	filter("configurations:Debug_Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="Debug_Editor"', "_EDITOR"}
		flags {"FatalWarnings"}

	filter ("configurations:Retail")
		runtime "Release"
		symbols "Off"
		optimize "Full"
		defines {'BUILD_CONFIG="Retail"', "NDEBUG"}
		flags {"FatalWarnings"}

	filter ("configurations:Retail", "action:vs*")
		buildoptions {"/wd4189"}
		flags {"FatalWarnings"}

	filter("configurations:Tests")
		runtime "Debug" 
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="Debug_Editor"', "_EDITOR", "_DEBUG"}
		linkoptions {"/IGNORE:4099"}

	--filter "system:windows"
	--	symbols "On"		
	--	systemversion "latest"
	--	defines {"_Win32", "_Win64"}

	filter {}
	flags {"MultiProcessorCompile"}
    warnings "Extra"
    externalanglebrackets "On"
	disablewarnings { "4244", "4018", "4100", "4189" }
	linkoptions {"/IGNORE:4006"}
    filter {}




--Source Root Directories
dirs["ThirdParty"]			= os.realpath(dirs.Source .. "ThirdParty/")
dirs["Runtime"]			    = os.realpath(dirs.Source .. "Runtime/")
dirs["Editor"]			    = os.realpath(dirs.Source .. "Editor/")
dirs["EntryPoint"]			= os.realpath(dirs.Source .. "EntryPoint/")
dirs["Sandbox"]				= os.realpath(dirs.Source .. "Sandbox/Public/")
dirs["Tests"]				= os.realpath(dirs.Source .. "Tests/")

--Inluding all thirdparty standalone libs
include (dirs.ThirdParty)

--Including all runtime libs
include (dirs.Runtime)

--Including all editor libs
include (dirs.Editor)

--Last include
include (dirs.EntryPoint)

include (normalizePath(dirs.Sandbox))

include (dirs.Tests)