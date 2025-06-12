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
		"Core",
		"DDSTextureLoader",
		"Assimp",
		"Vulkan",
		"DXC",
	})

	linkDependencies("Renderer", 
	{
		"RawShaders",
		"DXGI",
		"Core",
		"DDSTextureLoader",
		"Assimp",
		"Vulkan",
		"DXC",
	})


	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	defines{
		"GRAPHICS_DEBUG_INFORMATION"
	}

	filter ("platforms:x64-windows")
		defines {"VK_USE_PLATFORM_WIN32_KHR"}
	filter{}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

		if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch Renderer!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end
