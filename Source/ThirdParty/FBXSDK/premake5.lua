print("Including ThirdParty/FBXSDK")

project "FBXSDK"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("FBXSDK", 
	{
		dirs.FBXSDK, 
		dirs.FBXSDK .. "include/"
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
		dirs.FBXSDK .. "lib/debug"
	}

	filter "configurations:Release"
		libdirs { 
			dirs.FBXSDK .. "lib/release"
		}