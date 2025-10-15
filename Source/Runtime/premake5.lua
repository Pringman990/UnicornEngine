--Runtime Root Directories
print("Including Runtime")

dirs["Core"]						= os.realpath(dirs.Runtime .. "Core/Public")
dirs["Engine"]						= os.realpath(dirs.Runtime .. "Engine/Public")
dirs["Renderer"]					= os.realpath(dirs.Runtime .. "Renderer/")
dirs["ECS"]							= os.realpath(dirs.Runtime .. "ECS/Public")

group "Runtime/Core"
include (normalizePath(dirs.Core))
group ""

group "Runtime/Engine"
include (normalizePath(dirs.Engine))
group ""

group "Runtime/Renderer"
include (dirs.Renderer)
group ""

group "Runtime/Engine"
include (normalizePath(dirs.ECS))
group ""