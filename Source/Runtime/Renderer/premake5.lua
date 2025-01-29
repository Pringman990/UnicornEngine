print("Including Runtime/Renderer Libraries/")

--Layer One
dirs["RawShaders"]					= os.realpath(dirs.Renderer .. "RawShaders/Public/")
include (normalizePath(dirs.RawShaders))

--Layer Last
dirs["Renderer"]				= os.realpath(dirs.Renderer .. "Renderer/Public/")
include (normalizePath(dirs.Renderer))