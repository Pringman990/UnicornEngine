print("Including EntryPoint")

project "EntryPoint"
	language "C++"
	cppdialect "C++20"
	kind "WindowedApp"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		--dirs.Inter_Generated .. "**.h",
		--dirs.Inter_Generated .. "**.hpp",
		--dirs.Inter_Generated .. "**.cpp",
		--dirs.Inter_Generated .. "**.c"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	includedirs {
		dirs.EntryPoint .. "Private",
	}

	includeDependencies("EntryPoint", 
	{
		dirs.EntryPoint .. "Public",
		"Core",
		"Renderer",
		"ECS",
	})

	linkDependencies("EntryPoint", 
	{
		"Core",
		"Renderer",
		"ECS",
	})

	dependson {
		"Sandbox"
	}

	filter("configurations:Debug_Editor")
		includedirs {
			dirs.Editor,
		}	
		links {
			"Editor"
		} 
	filter("configurations:MemoryDebug_Editor")
		includedirs {
			dirs.Editor,
		}	
		links {
			"Editor"
		} 
	filter("configurations:Tests")
		includedirs {
			dirs.Editor,
		}	
		links {
			"Editor"
		} 

		 filter {"configurations:MemoryDebug"}
      -- Additional post-build command to copy debug-specific DLLs
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

    filter ("configurations:MemoryDebug_Editor")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

    filter ("configurations:Debug")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }
      
    filter ("configurations:Debug_Editor")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

   filter "configurations:Retail"
      -- Additional post-build command to copy release-specific DLLs
      postbuildcommands {
         "echo Copying release DLLs from " .. UCE_DLL_DIR .. "/release to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\release\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }
      print("Output directory: " .. UCE_EXECUTABLE_DIR)

filter {}