--Runtime Root Directories
print("Including Runtime")

dirs["Singleton"]					= os.realpath(dirs.Runtime .. "Singleton/Public")
dirs["Core"]						= os.realpath(dirs.Runtime .. "Core/Public")
dirs["Renderer"]					= os.realpath(dirs.Runtime .. "Renderer/")
dirs["ECS"]							= os.realpath(dirs.Runtime .. "ECS/Public")

group "Runtime/Singleton"
include (normalizePath(dirs.Singleton))
group ""

group "Runtime/Core"
include (normalizePath(dirs.Core))
group ""

group "Runtime/Renderer"
include (dirs.Renderer)
group ""

group "Runtime/ECS"
include (normalizePath(dirs.ECS))
group ""