--Core Root Directories
print("Including Runtime/ECS/")

project "ECS"
	language "C++"
	cppdialect "C++20"
	kind "SharedLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR)
	implibdir(UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.ECS) .. "Private/"
	}

	includeDependencies("ECS", 
	{
		dirs.ECS,
		"Engine"
	})

	linkDependencies("ECS", 
	{
		"Engine",
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		"**.inl",
	}

	defines{
		"ECS_EXPORTS"
	}
	
	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}