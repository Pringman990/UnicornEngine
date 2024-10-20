print("Including ThirdParty/")

--ThirdParty Root Directories
dirs["SimpleMath"]			= os.realpath(dirs.ThirdParty .. "SimpleMath/")
dirs["Spdlog"]				= os.realpath(dirs.ThirdParty .. "Spdlog/Public/")
dirs["StackWalker"]			= os.realpath(dirs.ThirdParty .. "StackWalker/")
dirs["FBXImporter"]			= os.realpath(dirs.ThirdParty .. "FBXImporter/")
dirs["DDSTextureLoader"]	= os.realpath(dirs.ThirdParty .. "DDSTextureLoader/")
dirs["Imgui"]				= os.realpath(dirs.ThirdParty .. "Imgui/")

group "ThirdParty"
include (dirs.SimpleMath)
include (normalizePath(dirs.Spdlog))
include (dirs.StackWalker)
include (dirs.FBXImporter)
include (dirs.DDSTextureLoader)
include (dirs.Imgui)
group ""