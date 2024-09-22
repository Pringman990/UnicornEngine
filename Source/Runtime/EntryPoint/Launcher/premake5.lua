print("Including Runtime/EntryPoint/Launcher")

project "Launcher"
	language "C++"
	cppdialect "C++20"
	kind "WindowedApp"
	
    targetname(defaultTargetName)
    targetdir (dirs.Binaries_Win64)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.Launcher,
		normalizePath(dirs.Launcher) .. "/Private",
		inheritAndIncludeDirsFromProject("EngineCore"),
		inheritAndIncludeDirsFromProject("Core") 
	}

	projectInheritDirs["Launcher"] = flattenTable({
		dirs.Launcher, 
		inheritAndIncludeDirsFromProject("EngineCore"),
		inheritAndIncludeDirsFromProject("Core") 
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	links{
		"EngineCore",
		"Core"
	}