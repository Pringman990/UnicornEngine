print("Including Runtime/EngineCore/")

--EngineCore Root Directories

dirs["Engine"]						= os.realpath(dirs.EngineCore .. "Engine/Public")

include (normalizePath(dirs.Engine))


project "EngineCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		inheritAndIncludeDirsFromProject("Engine")
	}

	projectInheritDirs["EngineCore"] = flattenTable({
		inheritAndIncludeDirsFromProject("Engine")
	})

	links{
		"Engine"
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end