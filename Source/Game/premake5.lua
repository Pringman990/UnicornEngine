
project "Game"
	language "C++"
	cppdialect "C++17"
	kind "WindowedApp"
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	location (dirs.Local)
	targetdir (dirs.bin)
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
		dirs.include,
		dirs.graphics_engine,
		dirs.engine,
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


	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "Full"

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