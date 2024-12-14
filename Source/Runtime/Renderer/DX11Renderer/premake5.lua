print("Including Runtime/Renderer/DX11Renderer")

project "DX11Renderer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.DX11Renderer) .. "/Private"
	}

	includeDependencies("DX11Renderer", 
	{
		dirs.DX11Renderer,
		"RendererCore",
		"DDSTextureLoader"
	})

	linkDependencies("DX11Renderer", 
	{
		"RendererCore",
		"DXGI",
		"DDSTextureLoader"
	})

	defines{
		"GRAPHICS_DEBUG_INFORMATION"
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch DX11Renderer!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end