--Core Root Directories
print("Including Sandbox")

group "Sandbox"
project "Sandbox"
	language "C++"
	cppdialect "C++20"
	kind "SharedLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR) --We do executable dir because it generates on build
	implibdir(UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.Sandbox) .. "Private/"
	}

	includeDependencies("Sandbox", 
	{
		dirs.Sandbox,
		"Renderer",
		"Singleton",
		"Engine",
		"Core"
	})

	linkDependencies("Sandbox", 
	{
		"Renderer",
		"Singleton",
		"Engine",
		"Core"
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	defines{
		"SANDBOX_EXPORTS"
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

group ""

filter {}