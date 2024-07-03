
project "Game"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	
	location (dirs.Local)
	targetdir (dirs.temp)
	implibdir (dirs.lib)
	
	flags { "FatalWarnings" }

	warnings "Extra"
	disablewarnings { disabledwarnings }

	vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

	pchheader "GamePch.h"
	pchsource "GamePch.cpp"
	
	forceincludes { "GamePch.h" }

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	excludes {
		dirs.include .. "ffmpeg-2.0/**.h",
		dirs.include .. "ffmpeg-2.0/**.c",
		dirs.include .. "ffmpeg-2.0/**.cpp",
	}

	includedirs {
		dirs.game,
		dirs.game .. "source/",
		dirs.include,
		dirs.engine,
		dirs.engine .. "source/",
        dirs.graphics_engine,
        dirs.graphics_engine .. "source/",
        dirs.externals
	}

	dependson {
		"Engine",
        "GraphicsEngine",
		"Externals"
	}

	links{
        "Engine",
        "GraphicsEngine",
		"Externals"
    }

	libdirs { 
        dirs.lib, 
        dirs.temp 
    }
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines {"_DEBUG"}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"
		defines {"_RELEASE", "NDEBUG"}

		filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"

		flags { 
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}

	if not os.isfile("GamePch.h") then
        io.writefile("GamePch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"GamePch Game!\")\n\n"
        )
    end

    if not os.isfile("GamePch.cpp") then
        io.writefile("GamePch.cpp", 
        "#include \"GamePch.h\"")
    end