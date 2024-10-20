print("Including ThirdParty/FBXImporter")

project "FBXImporter"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.FBXImporter,
		dirs.FBXImporter .. "FBXSDK/include/"
	}

	projectInheritDirs["FBXImporter"] = flattenTable({
		dirs.FBXImporter,
	})


	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"libfbxsdk"
	}

	defines{
		"FBXSDK_SHARED"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	
		libdirs { 
			dirs.FBXImporter .. "FBXSDK/lib/debug"
		}

	filter "configurations:Release"
		libdirs { 
			dirs.FBXImporter .. "FBXSDK/lib/release"
		}