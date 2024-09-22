print("Including Runtime/Core/Logger")

project "Logger"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.Logger,
		normalizePath(dirs.Logger) .. "/Private",
		inheritAndIncludeDirsFromProject("Spdlog"),
		inheritAndIncludeDirsFromProject("STLWrapper"),
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
	}

	projectInheritDirs["Logger"] = flattenTable({
		dirs.Logger,
		inheritAndIncludeDirsFromProject("Spdlog"),
		inheritAndIncludeDirsFromProject("STLWrapper"),
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
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
		"Spdlog",
		"STLWrapper",
		"MemoryDebugger"
	}