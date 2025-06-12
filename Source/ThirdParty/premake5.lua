print("Including ThirdParty/")

--ThirdParty Root Directories
dirs["SimpleMath"]			= os.realpath(dirs.ThirdParty .. "SimpleMath/")
dirs["Spdlog"]				= os.realpath(dirs.ThirdParty .. "Spdlog/")
dirs["StackWalker"]			= os.realpath(dirs.ThirdParty .. "StackWalker/")
dirs["FBXSDK"]				= os.realpath(dirs.ThirdParty .. "FBXSDK/")
dirs["Assimp"]				= os.realpath(dirs.ThirdParty .. "Assimp/")
dirs["DDSTextureLoader"]	= os.realpath(dirs.ThirdParty .. "DDSTextureLoader")
dirs["Imgui"]				= os.realpath(dirs.ThirdParty .. "Imgui/")
dirs["GoogleTest"]			= os.realpath(dirs.ThirdParty .. "GoogleTest/")
dirs["Vulkan"]				= os.realpath(dirs.ThirdParty .. "Vulkan/")
dirs["SDL"]					= os.realpath(dirs.ThirdParty .. "SDL/")
dirs["DXC"]					= os.realpath(dirs.ThirdParty .. "DXC/")

group "ThirdParty"
include (dirs.SimpleMath)
include (dirs.Spdlog)
include (dirs.StackWalker)
include (dirs.FBXSDK)
include (dirs.Assimp)
include (dirs.DDSTextureLoader)
include (dirs.Imgui)
include (dirs.GoogleTest)
include (dirs.Vulkan)
include (dirs.SDL)
include (dirs.DXC)
group ""