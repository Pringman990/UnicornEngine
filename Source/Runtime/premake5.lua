--Runtime Root Directories
print("Including Runtime")

dirs["Core"]						= os.realpath(dirs.Runtime .. "Core/")
dirs["ResourceManagment"]			= os.realpath(dirs.Runtime .. "ResourceManagment/")
dirs["EngineCore"]					= os.realpath(dirs.Runtime .. "EngineCore/")
dirs["EntryPoint"]					= os.realpath(dirs.Runtime .. "EntryPoint/")

group "Runtime/Core"
include (dirs.Core)

group "Runtime/ResourceManagment"
include (dirs.ResourceManagment)

group "Runtime/EngineCore"
include (dirs.EngineCore)


--Last Inlcude
group "Runtime/EntryPoint"
include (dirs.EntryPoint)

group ""
