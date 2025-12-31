print("Including ThirdParty/Imgui")

project "Imgui"
	language "C++"
	cppdialect "C++20"
	kind "SharedLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR)
	implibdir(UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)
	
	includeDependencies("Imgui", 
	{
		dirs.Imgui,
		dirs.Imgui .. "backends/",
		dirs.Imgui .. "misc/cpp/",
		dirs.Imgui .. "misc/single_file/",
		dirs.Vulkan .. "Include/"
	});

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	links{
		"Vulkan",
		"DXGI",
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}
	
	defines{"IMGUI_EXPORTS"}