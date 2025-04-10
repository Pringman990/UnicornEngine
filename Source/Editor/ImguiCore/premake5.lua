print("Including Editor/ImguiCore")

project "ImguiCore"
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

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	includedirs {
		normalizePath(dirs.ImguiCore) .. "/Private",
	}

	includeDependencies("ImguiCore", 
	{
		dirs.ImguiCore,
		"Imgui",
		"Renderer",
		"Core"
	})

	linkDependencies("ImguiCore", 
	{
		"Imgui",
		"Renderer",
		"Core"
	})

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch ImguiCore!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end

filter {}