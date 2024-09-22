print("Including Runtime/Core/Timer")

project "Timer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.Timer,
		normalizePath(dirs.Timer) .. "../Private",
		inheritAndIncludeDirsFromProject("STLWrapper")
	}

	projectInheritDirs["Timer"] = flattenTable({
		dirs.Timer,
		inheritAndIncludeDirsFromProject("STLWrapper")
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"STLWrapper"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

