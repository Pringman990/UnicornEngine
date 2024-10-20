print("Including Runtime/Renderer/ShadersCore")

project "ShadersCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
   targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.ShadersCore,
		normalizePath(dirs.ShadersCore) .. "../Private",
	}

	projectInheritDirs["ShadersCore"] = flattenTable({
		dirs.ShadersCore,
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	
	links{
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

