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
        "Retail"
    }

	platforms{ 
		"Win64"
	}

	filter("platforms:Win64")
		architecture "x64"

  	filter ("configurations:MemoryDebug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="MemoryDebug"', "_MEMORY_DEBUG", "_DEBUG"}

	filter ("configurations:MemoryDebug_Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="MemoryDebug_Editor"', "_MEMORY_DEBUG", "_DEBUG", "_EDITOR"}

    filter ("configurations:Debug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="Debug"', "_DEBUG"}

	filter("configurations:Debug_Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {'BUILD_CONFIG="Debug_Editor"', "_EDITOR"}

	filter ("configurations:Retail")
		runtime "Release"
		symbols "Off"
		optimize "Full"
		defines {'BUILD_CONFIG="Retail"', "NDEBUG"}

	filter ("configurations:Retail", "action:vs*")
		buildoptions {"/wd4189"}

	filter "system:windows"
		symbols "On"		
		systemversion "latest"
		defines {"_Win32", "_Win64"}

	flags {"FatalWarnings", "MultiProcessorCompile"}
    warnings "Extra"
    externalanglebrackets "On"
	disablewarnings { "4244", "4018", "4100", "4189" }
    filter {}




--Source Root Directories
dirs["ThirdParty"]			= os.realpath(dirs.Source .. "ThirdParty/")
dirs["Runtime"]			    = os.realpath(dirs.Source .. "Runtime/")
dirs["Editor"]			    = os.realpath(dirs.Source .. "Editor/")
dirs["EntryPoint"]			= os.realpath(dirs.Source .. "EntryPoint/")
dirs["Sandbox"]				= os.realpath(dirs.Source .. "Sandbox/Public/")

--Inluding all thirdparty standalone libs
include (dirs.ThirdParty)

--Including all runtime libs
include (dirs.Runtime)

--Including all editor libs
include (dirs.Editor)

--Last include
include (dirs.EntryPoint)

include (normalizePath(dirs.Sandbox))