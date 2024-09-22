print("Including Source/")

include "../Scripts/Premake/common.lua"

solution "Unicorn Engine"
    location "../"
    startproject "Launcher"
    architecture "x64"

    configurations {
        "MemoryDebug",
        "MemoryDebug Editor",
        "Debug",
        "Debug Editor",
        "Development Editor",
        "Retail"
    }

  	filter ("configurations:MemoryDebug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {"_MEMORY_DEBUG", "_DEBUG"}

    filter ("configurations:MemoryDebug Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {"_MEMORY_DEBUG", "_DEBUG", "_EDITOR"}

    filter ("configurations:Debug")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {"_DEBUG"}

	filter ("configurations:Debug Editor")
		runtime "Debug"
		symbols "Full"
		optimize "Off"
		defines {"_DEBUG", "_EDITOR"}

	filter ("configurations:Development Editor")
		runtime "Debug"
		symbols "On"
		optimize "Debug"
		defines {"_DEVELOPMENT", "_EDITOR"}

	filter ("configurations:Retail")
		runtime "Release"
		symbols "Off"
		optimize "Full"
		defines {"NDEBUG"}

	filter ("configurations:Retail", "action:vs*")
		buildoptions {"/wd4189"}

	filter "system:windows"
		symbols "On"		
		systemversion "latest"
		defines {"_Win32"}

	flags {"FatalWarnings", "MultiProcessorCompile"}
    warnings "Extra"
    externalanglebrackets "On"

    filter {}


--Source Root Directories
dirs["ThirdParty"]			= os.realpath(dirs.Source .. "ThirdParty/")
dirs["Runtime"]			    = os.realpath(dirs.Source .. "Runtime/")

--Inluding all thirdparty standalone libs
include (dirs.ThirdParty)

--Including all runtime libs
include (dirs.Runtime)
