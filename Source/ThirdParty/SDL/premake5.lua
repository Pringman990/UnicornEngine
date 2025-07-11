print("Including ThirdParty/SDL")

project "SDL"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("SDL", 
	{
		dirs.SDL, 
		dirs.SDL .. "Include/"
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"SDL2"
	}

	defines{
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	
	libdirs { 
		dirs.SDL .. "lib"
	}