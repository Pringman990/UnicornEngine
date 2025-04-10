--Core Root Directories
print("Including Editor/")

group "Editor/Core"
--Layer One
dirs["ImguiCore"]				= os.realpath(dirs.Editor .. "ImguiCore/Public/")
include (normalizePath(dirs.ImguiCore))

--Last Layer
dirs["Editor"]					= os.realpath(dirs.Editor .. "Editor/Public/")
include (normalizePath(dirs.Editor))

group ""