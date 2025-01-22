print("Including Runtime/Renderer Libraries/")

--Layer One
dirs["RendererCore"]				= os.realpath(dirs.Renderer .. "RendererCore/Public/")
dirs["RawShaders"]					= os.realpath(dirs.Renderer .. "RawShaders/Public/")
include (normalizePath(dirs.RendererCore))
include (normalizePath(dirs.RawShaders))

--Layer Two
dirs["DX11Renderer"]				= os.realpath(dirs.Renderer .. "DX11Renderer/Public/")
include (normalizePath(dirs.DX11Renderer))

--Layer Last
dirs["Renderer"]					= os.realpath(dirs.Renderer .. "Renderer/Public/")
include (normalizePath(dirs.Renderer))