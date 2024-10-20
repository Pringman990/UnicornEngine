print("Including Runtime/Renderer/RendererFactory")

project "RendererFactory"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
   targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.RendererFactory,
		normalizePath(dirs.RendererFactory) .. "../Private",
		inheritAndIncludeDirsFromProject("DX11Renderer"),
		inheritAndIncludeDirsFromProject("RendererCore"),
	}

	projectInheritDirs["RendererFactory"] = flattenTable({
		dirs.RendererFactory,
		inheritAndIncludeDirsFromProject("DX11Renderer"),
		inheritAndIncludeDirsFromProject("RendererCore"),
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	
	links{
		"DX11Renderer",
		"RendererCore"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

