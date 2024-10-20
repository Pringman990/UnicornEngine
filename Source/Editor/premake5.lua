--Core Root Directories
print("Including Editor/")

group "Editor/Core"
--Layer One
dirs["ImguiCore"]				= os.realpath(dirs.Editor .. "ImguiCore/Public/")

include (normalizePath(dirs.ImguiCore))

--Last Layer
dirs["EditorCore"]				= os.realpath(dirs.Editor .. "EditorCore/Public/")

include (normalizePath(dirs.EditorCore))

group ""

group "Editor"
project "Editor"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		--Layer One
		inheritAndIncludeDirsFromProject("ImguiCore"),

		--Last Layer
		inheritAndIncludeDirsFromProject("EditorCore")
	}

	projectInheritDirs["Editor"] = flattenTable({
		--Layer One
		inheritAndIncludeDirsFromProject("ImguiCore"),

		--Last Layer
		inheritAndIncludeDirsFromProject("EditorCore")
	})

	links{
		--Layer One
		"ImguiCore",

		--Last Layer
		"EditorCore",
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end

	group ""