--Core Root Directories
print("Including Runtime/Renderer/")

--Layer One
dirs["RendererCore"]				= os.realpath(dirs.Renderer .. "RendererCore/Public/")
dirs["RawShaders"]					= os.realpath(dirs.Renderer .. "RawShaders/Public/")

include (normalizePath(dirs.RendererCore))
include (normalizePath(dirs.RawShaders))

--Layer Two
dirs["ShadersCore"]					= os.realpath(dirs.Renderer .. "ShadersCore/Public/")

include (normalizePath(dirs.ShadersCore))

--Layer Three
dirs["DX11Renderer"]				= os.realpath(dirs.Renderer .. "DX11Renderer/Public/")

include (normalizePath(dirs.DX11Renderer))

--Layer Last
dirs["RendererFactory"]		= os.realpath(dirs.Renderer .. "RendererFactory/Public/")

include (normalizePath(dirs.RendererFactory))

project "Renderer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		--Layer One
		inheritAndIncludeDirsFromProject("RendererCore"),
		inheritAndIncludeDirsFromProject("RawShaders"),

		--Layer Two
		inheritAndIncludeDirsFromProject("ShadersCore"),

		--Layer Three
		inheritAndIncludeDirsFromProject("DX11Renderer"),

		--Layer Last
		inheritAndIncludeDirsFromProject("RendererFactory"),
	}

	projectInheritDirs["Renderer"] = flattenTable({
		--Layer One
		inheritAndIncludeDirsFromProject("RendererCore"),
		inheritAndIncludeDirsFromProject("RawShaders"),

		--Layer Two
		inheritAndIncludeDirsFromProject("ShadersCore"),

		--Layer Three
		inheritAndIncludeDirsFromProject("DX11Renderer"),

		--Layer Last
		inheritAndIncludeDirsFromProject("RendererFactory"),


	})

	links{
		--Layer One
		"RendererCore",
		"RawShaders",

		--Layer Two
		"ShadersCore",

		--Layer Three
		"DX11Renderer",

		--Layer Last
		"RendererFactory",
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end