--Core Root Directories
print("Including Editor/")

group "Editor/Core"
--Layer One
dirs["ImguiCore"]				= os.realpath(dirs.Editor .. "ImguiCore/Public/")
include (normalizePath(dirs.ImguiCore))

--Last Layer
dirs["EditorCore"]				= os.realpath(dirs.Editor .. "Editor/Public/")
include (normalizePath(dirs.EditorCore))

dirs["GameCoreEditor"]			= os.realpath(dirs.Editor .. "GameCoreEditor/Public/")
include (normalizePath(dirs.GameCoreEditor))

group ""