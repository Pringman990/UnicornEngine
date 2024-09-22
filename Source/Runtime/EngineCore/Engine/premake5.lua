print("Including Runtime/EngineCore/Engine")

project "Engine"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
    targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)
		
	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	includedirs {
		dirs.Engine,
		normalizePath(dirs.Engine) .. "../Private",
		inheritAndIncludeDirsFromProject("Core")
	}

	projectInheritDirs["Engine"] = flattenTable({
		dirs.Engine,
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
		"Core"
	}

    if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch Engine!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end