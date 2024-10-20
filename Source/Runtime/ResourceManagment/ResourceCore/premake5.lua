print("Including Runtime/Core/ResourceCore")

project "ResourceCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.ResourceCore,
		normalizePath(dirs.ResourceCore) .. "/Private/",
		inheritAndIncludeDirsFromProject("Core"),
	}

	projectInheritDirs["ResourceCore"] = flattenTable({
		dirs.ResourceCore,
		inheritAndIncludeDirsFromProject("Core"),
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
		"Core"
	}