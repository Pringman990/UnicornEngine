print("Including Runtime/EntryPoint/")

--EntryPoint Root Directories

dirs["Launcher"]			= os.realpath(dirs.EntryPoint .. "Launcher/Public")

include (normalizePath(dirs.Launcher))

project "EntryPoint"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		inheritAndIncludeDirsFromProject("Launcher")
	}

	projectInheritDirs["EntryPoint"] = flattenTable({
		inheritAndIncludeDirsFromProject("Launcher")
	})

	links{
		"Launcher"
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end