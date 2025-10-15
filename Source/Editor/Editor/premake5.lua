print("Including Editor/Editor")

project "Editor"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

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
		normalizePath(dirs.Editor) .. "/Private",
	}

	includeDependencies("Editor", 
	{
		dirs.Editor,
		"ImguiCore",
		"Renderer",
		"Engine",
		"ECS",
	})

	linkDependencies("Editor", 
	{
		"ImguiCore",
		"Renderer",
		"Engine",
		"ECS",
	})

filter {}