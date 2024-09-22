print("Including ThirdParty/SimpleMath")

project "SimpleMath"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	includedirs {
		dirs.SimpleMath
	}

	projectInheritDirs["SimpleMath"] = flattenTable({
		dirs.SimpleMath,
	})

	dependson{}
	links{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}
