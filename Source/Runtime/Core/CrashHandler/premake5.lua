print("Including Runtime/Core/CrashHandler")

project "CrashHandler"
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
		dirs.CrashHandler,
		normalizePath(dirs.CrashHandler) .. "../Private"
	}

	projectInheritDirs["CrashHandler"] = flattenTable({
		dirs.CrashHandler
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

    if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch CrashHandler!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end