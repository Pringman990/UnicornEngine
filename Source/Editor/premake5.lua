
project "Editor"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    objdir("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
    targetname("%{prj.name}_%{cfg.buildcfg}")

    location (dirs.Local)
    targetdir (dirs.temp)
	implibdir (dirs.lib)

    flags { "FatalWarnings" }

    warnings "Extra"
    disablewarnings { disabledwarnings }

    vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

    pchheader "EditorPch.h"
	pchsource "EditorPch.cpp"

    forceincludes { "EditorPch.h" }

    includedirs {
        ".",
        dirs.include,
		dirs.engine,
		dirs.engine .. "source/",
        dirs.graphics_engine,
        dirs.graphics_engine .. "source/",
        dirs.editor,
        dirs.editor .. "source/",
        dirs.externals
    }
    
    	dependson {
		"Engine",
        "GraphicsEngine",
		"Externals"
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

    links{
        "Engine",
        "GraphicsEngine",
		"Externals"
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

    if not os.isfile("EditorPch.h") then
        io.writefile("EditorPch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"EditorPch Editor!\")\n\n"
        )
    end

    if not os.isfile("EditorPch.cpp") then
        io.writefile("EditorPch.cpp", 
        "#include \"EditorPch.h\"")
    end