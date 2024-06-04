
project "Engine"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	location (dirs.Local)
	targetdir (dirs.temp)
	implibdir (dirs.lib)
	
	flags { "FatalWarnings" }

	warnings "Extra"
	disablewarnings { disabledwarnings }

	vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

	pchheader "EnginePch.h"
	pchsource "EnginePch.cpp"

	forceincludes { "EnginePch.h" }

	includedirs {
		".",
		dirs.include,
		dirs.engine,
		dirs.graphics_engine,
        dirs.externals
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	   libdirs { 
        dirs.lib, 
        dirs.temp 
    }

	dependson{
		"GraphicsEngine",
		"Externals"
	}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "Full"

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

	if not os.isfile("EnginePch.h") then
        io.writefile("EnginePch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"EnginePch Engine!\")\n\n"
        )
    end

    if not os.isfile("EnginePch.cpp") then
        io.writefile("EnginePch.cpp", 
        "#include \"EnginePch.h\"")
    end