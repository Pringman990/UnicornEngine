print("Including ThirdParty/")

--ThirdParty Root Directories
dirs["SimpleMath"]		= os.realpath(dirs.ThirdParty .. "SimpleMath/")
dirs["Spdlog"]			= os.realpath(dirs.ThirdParty .. "Spdlog/Public/")
dirs["StackWalker"]		= os.realpath(dirs.ThirdParty .. "StackWalker/")

group "ThirdParty"
include (dirs.SimpleMath)
include (normalizePath(dirs.Spdlog))
include (dirs.StackWalker)
group ""