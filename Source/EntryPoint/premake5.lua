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
		"**.c"
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
		"EngineCore"
	})

	linkDependencies("EntryPoint", 
	{
		"Core",
		"Renderer",
		"EngineCore"
	})

	filter("configurations:Debug Editor")
		includedirs {
			dirs.Editor,
			dirs.EditorCore
		}	
		links {
			"Editor"
		} 
	filter("configurations:MemoryDebug Editor")
		includedirs {
			dirs.Editor,
			dirs.EditorCore
		}	
		links {
			"Editor"
		} 

filter {}