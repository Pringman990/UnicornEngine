
project "GraphicsEngine"
	language "C++"
	cppdialect "C++20"
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

    pchheader "GraphicsPch.h"
	pchsource "GraphicsPch.cpp"

	dependson {
		"Shaders",
		"Engine",
		"Externals"
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		"**.hlsl",
		"**.hlsli"
	}

	forceincludes { "GraphicsPch.h" }

	includedirs {
		".",
		dirs.graphics_engine,
		dirs.graphics_engine .. "source/",
		dirs.engine,
		dirs.shaders,
		dirs.include,
        dirs.externals
	}

	libdirs { 
		dirs.lib 
	}

		defines{
		"GRAPHICS_DEBUG_INFORMATION"
	}



	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

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

	if not os.isfile("GraphicsPch.h") then
        io.writefile("GraphicsPch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"GraphicsPch Graphics Engine\")\n\n"
        )
    end

    if not os.isfile("GraphicsPch.cpp") then
        io.writefile("GraphicsPch.cpp", 
        "#include \"GraphicsPch.h\"")
    end