
project "Launcher"
    language "C++"
    cppdialect "C++20"
    kind "WindowedApp"
    objdir("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
    targetname("%{prj.name}_%{cfg.buildcfg}")

    location (dirs.Local)
	targetdir (dirs.bin)
	implibdir (dirs.lib)

    flags { "FatalWarnings" }

    warnings "Extra"
    disablewarnings { disabledwarnings }

    vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

    pchheader "LauncherPch.h"
	pchsource "LauncherPch.cpp"

    forceincludes { "LauncherPch.h" }

    includedirs {
        ".",
        dirs.include,
		dirs.engine,
		dirs.engine .. "source/",
        dirs.graphics_engine,
        dirs.graphics_engine .. "source/",
        dirs.game,
        dirs.shaders,
        dirs.externals
    }
    
    dependson {
		"Engine",
		"Game",
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
		"Game",
        "GraphicsEngine",
		"Externals"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines {"_DEBUG"}
        dependson {"Editor"}
        includedirs{ dirs.editor }
        links {  
            "Editor_Debug.lib"
        } 

    filter "configurations:Release"
        runtime "Release"
        optimize "Full"
        defines {"_RELEASE", "NDEBUG"}

        filter "system:windows"
		kind "WindowedApp"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"

		flags { 
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}

    if not os.isfile("LauncherPch.h") then
        io.writefile("LauncherPch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"LauncherPch Editor!\")\n\n"
        )
    end

    if not os.isfile("LauncherPch.cpp") then
        io.writefile("LauncherPch.cpp", 
        "#include \"LauncherPch.h\"")
    end

  -- Ensure the outputdir has the correct format
   print("Output directory: " .. dirs.bin)

   -- Common post-build command to copy general DLLs
   postbuildcommands {
      "echo Copying general DLLs from " .. dirs.DLL .. " to " .. dirs.bin,
      "xcopy /Q /Y /I \"" .. dirs.DLL .. "\\*.dll\" \"" .. dirs.bin .. "\\\""
   }

   filter "configurations:Debug"
      -- Additional post-build command to copy debug-specific DLLs
      postbuildcommands {
         "echo Copying debug DLLs from " .. dirs.DLL .. "/debug to " .. dirs.bin,
         "xcopy /Q /Y /I \"" .. dirs.DLL .. "\\debug\\*.dll\" \"" .. dirs.bin .. "\\\""
      }

   filter "configurations:Release"
      -- Additional post-build command to copy release-specific DLLs
      postbuildcommands {
         "echo Copying release DLLs from " .. dirs.DLL .. "/release to " .. dirs.bin,
         "xcopy /Q /Y /I \"" .. dirs.DLL .. "\\release\\*.dll\" \"" .. dirs.bin .. "\\\""
      }