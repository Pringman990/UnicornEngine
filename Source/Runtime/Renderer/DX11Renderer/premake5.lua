print("Including Runtime/Renderer/DX11Renderer")

project "DX11Renderer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
   targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.DX11Renderer,
		normalizePath(dirs.DX11Renderer) .. "../Private",
		inheritAndIncludeDirsFromProject("RendererCore"),
		--inheritAndIncludeDirsFromProject("ShaderCore"),
	}

	projectInheritDirs["DX11Renderer"] = flattenTable({
		dirs.DX11Renderer,
		inheritAndIncludeDirsFromProject("RendererCore"),
		--inheritAndIncludeDirsFromProject("ShaderCore"),
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	
	links{
		"RendererCore",
		"DXGI",
		--"ShaderCore"
	}

	defines{
		"GRAPHICS_DEBUG_INFORMATION"
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