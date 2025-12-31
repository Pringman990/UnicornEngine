print("Including Editor/GameCoreEditor")

project "GameCoreEditor"
	language "C++"
	cppdialect "C++20"
	kind "SharedLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_EXECUTABLE_DIR)
	implibdir(UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		"**.inl",
	}

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}

	includedirs {
		normalizePath(dirs.GameCoreEditor) .. "Private/",
	}

	includeDependencies("GameCoreEditor", 
	{
		dirs.GameCoreEditor,
		"Editor",
		"GameCore",
	})

	linkDependencies("GameCoreEditor", 
	{
		"Editor",
		"GameCore",
	})

	defines{"GAMECOREEDITOR_EXPORTS"}

filter {}