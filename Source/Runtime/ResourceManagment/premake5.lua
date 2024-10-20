--ResourceManagment Root Directories
print("Including Runtime/ResourceManagment/")

--Layer Zero
dirs["ResourceCore"]				= os.realpath(dirs.ResourceManagment .. "ResourceCore/Public/")

include (normalizePath(dirs.ResourceCore))

project "ResourceManagment"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		--Layer Zero
		inheritAndIncludeDirsFromProject("ResourceCore"),
	}

	projectInheritDirs["ResourceManagment"] = flattenTable({
		--Layer Zero
		inheritAndIncludeDirsFromProject("ResourceCore"),
	})

	links{
		--Layer Zero
		"ResourceCore",
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end