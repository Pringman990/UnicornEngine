print("Including ThirdParty/Assimp")

project "Assimp"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs{
		dirs.Assimp .. "assimp/",
	}

	includeDependencies("Assimp", 
	{
		dirs.Assimp,
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"assimp-vc143-mtd"
	}

	defines{
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	
	libdirs { 
		dirs.Assimp .. "lib/debug"
	}

	filter "configurations:Release"
		libdirs { 
			dirs.Assimp .. "lib/release"
		}