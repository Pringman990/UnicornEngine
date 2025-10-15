--Core Root Directories
print("Including Runtime/Core/")

project "Core"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.Core) .. "/Private",
	}

	includeDependencies("Core", 
	{
		dirs.Core,
		"SimpleMath",
	})

	linkDependencies("Core", 
	{
		"SimpleMath",
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	filter{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}