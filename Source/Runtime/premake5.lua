--Runtime Root Directories
print("Including Runtime")

dirs["Core"]						= os.realpath(dirs.Runtime .. "Core/")
dirs["ResourceCore"]				= os.realpath(dirs.Runtime .. "ResourceCore/")
dirs["GUIFramework"]				= os.realpath(dirs.Runtime .. "GUIFramework/")
dirs["Renderer"]					= os.realpath(dirs.Runtime .. "Renderer/")
dirs["ECS"]							= os.realpath(dirs.Runtime .. "ECS/")
dirs["EngineCore"]					= os.realpath(dirs.Runtime .. "EngineCore/")

group "Runtime/Core"
include (dirs.Core)
group ""

group "Runtime/ResourceCore"
include (dirs.ResourceCore)
group ""

group "Runtime/GUIFramework"
include (dirs.GUIFramework)
group ""

group "Runtime/Renderer"
include (dirs.Renderer)
group ""

group "Runtime/ECS"
include (dirs.ECS)
group ""

group "Runtime/EngineCore"
include (dirs.EngineCore)
group ""