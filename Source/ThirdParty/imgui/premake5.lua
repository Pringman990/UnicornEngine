print("Including ThirdParty/Imgui")

project "Imgui"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)
	
	includeDependencies("Imgui", 
	{
		dirs.Imgui,
		dirs.Imgui .. "backends/",
		dirs.Imgui .. "misc/cpp/",
		dirs.Imgui .. "misc/single_file/",
	});

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
	