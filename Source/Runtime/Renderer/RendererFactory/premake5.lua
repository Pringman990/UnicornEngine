print("Including Runtime/Renderer/RendererFactory")

project "RendererFactory"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	includedirs {
		normalizePath(dirs.RendererFactory) .. "/Private"
	}

	includeDependencies("RendererFactory", 
	{
		dirs.RendererFactory,
		"DX11Renderer",
		"RendererCore"
	})

	linkDependencies("RendererFactory", 
	{
		"DX11Renderer",
		"RendererCore"
	})

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

