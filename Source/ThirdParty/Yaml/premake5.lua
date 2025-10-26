print("Including ThirdParty/Yaml")

project "Yaml"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("Yaml", 
	{
		dirs.Yaml
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"yaml-cppd",
	}

	defines{
		"YAML_CPP_STATIC_DEFINE"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	
	libdirs { 
		dirs.Yaml .. "lib"
	}

	filter "configurations:Release"
		links{
			"yaml-cpp",
		}

filter{}