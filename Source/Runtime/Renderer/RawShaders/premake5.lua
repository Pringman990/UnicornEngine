print("Including Runtime/Renderer/RawShaders")

project "RawShaders"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
   targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.RawShaders,
		normalizePath(dirs.RawShaders) .. "../Private",
	}

	projectInheritDirs["RawShaders"] = flattenTable({
		dirs.RawShaders,
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

