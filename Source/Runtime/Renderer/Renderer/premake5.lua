--Core Root Directories
print("Including Runtime/Renderer/")

project "Renderer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.Renderer) .. "/Private"
	}

	includeDependencies("Renderer", 
	{
		dirs.Renderer,
		"RawShaders",
		"Engine",
		"DDSTextureLoader",
		"Assimp",
		"Vulkan",
		"DXC",
		"STB",
	})

	linkDependencies("Renderer", 
	{
		"RawShaders",
		"DXGI",
		"Engine",
		"DDSTextureLoader",
		"Assimp",
		"Vulkan",
		"DXC",
		"STB",
	})


	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	defines{
		"GRAPHICS_DEBUG_INFORMATION",
		"STB_IMAGE_IMPLEMENTATION"
	}

	filter ("platforms:x64-windows")
		defines {"VK_USE_PLATFORM_WIN32_KHR"}
	filter{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}
