----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root    .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root    .. "Temp/")
dirs["Local"]			= os.realpath(dirs.root    .. "Local/")
dirs["lib"]				= os.realpath(dirs.root    .. "Lib/")
dirs["DLL"]				= os.realpath(dirs.root    .. "DLL/")
dirs["include"]			= os.realpath(dirs.root    .. "Include/")
dirs["source"] 			= os.realpath(dirs.root    .. "Source/")
dirs["engine"]			= os.realpath(dirs.source  .. "Engine/")
dirs["graphics_engine"]	= os.realpath(dirs.source  .. "GraphicsEngine/")
dirs["game"]			= os.realpath(dirs.source  .. "Game/")
dirs["editor"]			= os.realpath(dirs.source  .. "Editor/")
dirs["externals"]		= os.realpath(dirs.source  .. "Externals/")
dirs["shaders"]		    = os.realpath(dirs.source  .. "Shaders/")
dirs["shaders_output"]  = os.realpath(dirs.bin     .. "shaders/")
dirs["launcher"]		= os.realpath(dirs.source  .. "Launcher/")

disabledwarnings = { }