--GUIFramework Root Directories
print("Including Runtime/GUIFramework/")

--Layer One
--dirs["MemoryDebugger"]				= os.realpath(dirs.GUIFramework .. "MemoryDebugger/Public/")

--include (normalizePath(dirs.MemoryDebugger))

project "GUIFramework"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		--Layer One
	}

	links{
		--Layer One
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end