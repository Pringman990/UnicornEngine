print("Including Runtime/Core/MemoryDebugger")

project "MemoryDebugger"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.MemoryDebugger,
		normalizePath(dirs.MemoryDebugger) .. "../Private",
		inheritAndIncludeDirsFromProject("StackWalker")
	}

	projectInheritDirs["MemoryDebugger"] = flattenTable({
		dirs.MemoryDebugger,
		inheritAndIncludeDirsFromProject("StackWalker")
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	links{
		"StackWalker"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

