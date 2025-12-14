--Core Root Directories
print("Including Runtime/GameCore/")

project "GameCore"
	language "C++"
	cppdialect "C++20"
	kind "SharedLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR)
	implibdir(UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.GameCore) .. "/Private",
	}

	includeDependencies("GameCore", 
	{
		dirs.GameCore,
		"ECS",
		"Engine",
		"Renderer",
		"Core",
	})

	linkDependencies("GameCore", 
	{
		"ECS",
		"Engine",
		"Renderer",
		"Core",
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		"**.inl",
	}

	defines{
		"GAMECORE_EXPORTS"
	}
	
	filter{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}