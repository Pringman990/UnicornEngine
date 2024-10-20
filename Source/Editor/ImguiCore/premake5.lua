print("Including Editor/ImguiCore")

project "ImguiCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.ImguiCore,
		normalizePath(dirs.ImguiCore) .. "../Private",
		inheritAndIncludeDirsFromProject("Imgui")
	}

	projectInheritDirs["ImguiCore"] = flattenTable({
		dirs.ImguiCore,
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
		"Imgui"
	}