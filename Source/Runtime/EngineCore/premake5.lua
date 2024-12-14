print("Including Runtime/EngineCore/")

--EngineCore Root Directories

dirs["Engine"]						= os.realpath(dirs.EngineCore .. "Engine/Public")

include (normalizePath(dirs.Engine))


project "EngineCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("EngineCore", 
	{
		dirs.Engine
	})

	linkDependencies("EngineCore", 
	{
		"Engine"
	})

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end