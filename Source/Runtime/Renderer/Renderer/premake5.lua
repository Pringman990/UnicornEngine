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
		"RendererCore",
		"RawShaders",
		"DX11Renderer",
	})

	linkDependencies("Renderer", 
	{
		"RendererCore",
		"RawShaders",
		"DX11Renderer",
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}