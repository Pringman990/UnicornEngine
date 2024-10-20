--GUIFramework Root Directories
print("Including Runtime/GUIFramework/")

--Layer One
--dirs["MemoryDebugger"]				= os.realpath(dirs.GUIFramework .. "MemoryDebugger/Public/")

--include (normalizePath(dirs.MemoryDebugger))

project "GUIFramework"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		--Layer One
	}

	projectInheritDirs["GUIFramework"] = flattenTable({
		--Layer One
	})

	links{
		--Layer One
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end