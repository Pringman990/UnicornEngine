--Runtime Root Directories
print("Including Runtime")

dirs["Core"]						= os.realpath(dirs.Runtime .. "Core/")
dirs["ResourceManagment"]			= os.realpath(dirs.Runtime .. "ResourceManagment/")
dirs["EngineCore"]					= os.realpath(dirs.Runtime .. "EngineCore/")
dirs["GUIFramework"]				= os.realpath(dirs.Runtime .. "GUIFramework/")
dirs["Renderer"]					= os.realpath(dirs.Runtime .. "Renderer/")
dirs["EntryPoint"]					= os.realpath(dirs.Runtime .. "EntryPoint/")

group "Runtime/Core"
include (dirs.Core)

group "Runtime/ResourceManagment"
include (dirs.ResourceManagment)

group "Runtime/EngineCore"
include (dirs.EngineCore)

group "Runtime/GUIFramework"
include (dirs.GUIFramework)

group "Runtime/Renderer"
include (dirs.Renderer)

--Last Inlcude
group "Runtime/EntryPoint"
include (dirs.EntryPoint)

group ""
