print("Including ThirdParty/Vulkan")

project "Vulkan"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("Vulkan", 
	{
		dirs.Vulkan, 
		dirs.Vulkan .. "Include/"
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"vulkan-1",
	}

	defines{
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	
	libdirs { 
		dirs.Vulkan .. "lib"
	}