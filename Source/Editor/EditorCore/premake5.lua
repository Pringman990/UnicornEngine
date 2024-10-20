print("Including Editor/EditorCore")

project "EditorCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.EditorCore,
		normalizePath(dirs.EditorCore) .. "../Private",
		inheritAndIncludeDirsFromProject("ImguiCore"),
		inheritAndIncludeDirsFromProject("Imgui")
	}

	projectInheritDirs["EditorCore"] = flattenTable({
		dirs.EditorCore,
		inheritAndIncludeDirsFromProject("ImguiCore"),
		inheritAndIncludeDirsFromProject("Imgui")
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	links{
		"ImguiCore",
		"Imgui"
	}