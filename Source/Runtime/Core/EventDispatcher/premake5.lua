print("Including Runtime/Core/EventDispatcher")

project "EventDispatcher"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.EventDispatcher,
		normalizePath(dirs.EventDispatcher) .. "/Private",
		inheritAndIncludeDirsFromProject("STLWrapper")
	}

	projectInheritDirs["EventDispatcher"] = flattenTable({
		dirs.EventDispatcher,
		inheritAndIncludeDirsFromProject("STLWrapper")
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
		"STLWrapper"
	}