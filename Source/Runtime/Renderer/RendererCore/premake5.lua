print("Including Runtime/Renderer/RendererCore")

project "RendererCore"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	includedirs {
		normalizePath(dirs.RendererCore) .. "/Private"
	}

	includeDependencies("RendererCore", 
	{
		dirs.RendererCore,
		"FBXSDK",
		"Assimp",
		"AssetCore",
		"Math",
		"Core"
	})

	linkDependencies("RendererCore", 
	{
		"FBXSDK",
		"Assimp",
		"AssetCore",
		"Core"
	})

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