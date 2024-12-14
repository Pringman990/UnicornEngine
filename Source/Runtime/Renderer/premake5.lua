--Core Root Directories
print("Including Runtime/Renderer/")

--Layer One
dirs["RendererCore"]				= os.realpath(dirs.Renderer .. "RendererCore/Public/")
dirs["RawShaders"]					= os.realpath(dirs.Renderer .. "RawShaders/Public/")

include (normalizePath(dirs.RendererCore))
include (normalizePath(dirs.RawShaders))

--Layer Two

--Layer Three
dirs["DX11Renderer"]				= os.realpath(dirs.Renderer .. "DX11Renderer/Public/")

include (normalizePath(dirs.DX11Renderer))

--Layer Last
dirs["RendererFactory"]				= os.realpath(dirs.Renderer .. "RendererFactory/Public/")

include (normalizePath(dirs.RendererFactory))

project "Renderer"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("Renderer", 
	{
		"RendererCore",
		"RawShaders",
		"DX11Renderer",
		"RendererFactory"
	})

	linkDependencies("Renderer", 
	{
		"RendererCore",
		"RawShaders",
		"DX11Renderer",
		"RendererFactory"
	})

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end