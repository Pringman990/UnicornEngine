print("Including Editor/ImguiCore")

project "ImguiCore"
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
		normalizePath(dirs.ImguiCore) .. "/Private",
	}

	includeDependencies("ImguiCore", 
	{
		dirs.ImguiCore,
		"Imgui",
		"Renderer",
		"Engine"
	})

	linkDependencies("ImguiCore", 
	{
		"Imgui",
		"Renderer",
		"Engine"
	})

filter {}