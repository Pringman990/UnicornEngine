print("Including Runtime/Renderer/RendererCore")

project "RendererCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
   targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		dirs.RendererCore,
		normalizePath(dirs.RendererCore) .. "../Private",
		inheritAndIncludeDirsFromProject("FBXImporter"),
		inheritAndIncludeDirsFromProject("ResourceManagment"),
		inheritAndIncludeDirsFromProject("Core")
	}

	projectInheritDirs["RendererCore"] = flattenTable({
		dirs.RendererCore,
		inheritAndIncludeDirsFromProject("FBXImporter"),
		inheritAndIncludeDirsFromProject("ResourceManagment"),
		inheritAndIncludeDirsFromProject("Core")
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	dependson{}
	
	links{
		"FBXImporter",
		"ResourceManagment",
		"Core"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}


	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch RendererCore!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end