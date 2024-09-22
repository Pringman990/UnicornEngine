print("Including ThirdParty/Spdlog")

project "Spdlog"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)
	
	includedirs {
		dirs.Spdlog,
		normalizePath(dirs.Spdlog) .. "../Private"
	}

	projectInheritDirs["Spdlog"] = flattenTable({
		dirs.Spdlog,
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	links{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}
	