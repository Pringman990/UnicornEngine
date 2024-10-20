print("Including Runtime/Core/UniqueID")

project "UniqueID"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.UniqueID,
		normalizePath(dirs.UniqueID) .. "/Private",
		inheritAndIncludeDirsFromProject("StandardTypes")
	}

	projectInheritDirs["UniqueID"] = flattenTable({
		dirs.UniqueID,
		inheritAndIncludeDirsFromProject("StandardTypes")
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
		"StandardTypes"
	}