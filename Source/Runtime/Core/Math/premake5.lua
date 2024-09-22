print("Including Runtime/Core/Math")

project "Math"
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
		dirs.Math,
		normalizePath(dirs.Math) .. "../Private",
		inheritAndIncludeDirsFromProject("SimpleMath")
	}

	projectInheritDirs["Math"] = flattenTable({
		dirs.Math,
		inheritAndIncludeDirsFromProject("SimpleMath")
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
		"SimpleMath"
	}

    if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch Math!\")\n\n" ..
		"#include <SimpleMath.h>\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end