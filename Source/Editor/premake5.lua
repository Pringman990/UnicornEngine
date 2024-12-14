--Core Root Directories
print("Including Editor/")

group "Editor/Core"
--Layer One
dirs["ImguiCore"]				= os.realpath(dirs.Editor .. "ImguiCore/Public/")

include (normalizePath(dirs.ImguiCore))

--Last Layer
dirs["EditorCore"]				= os.realpath(dirs.Editor .. "EditorCore/Public/")

include (normalizePath(dirs.EditorCore))

group ""

group "Editor"
project "Editor"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includeDependencies("Editor", 
	{
		"ImguiCore",
		"EditorCore"
	})

	linkDependencies("Editor", 
	{
		"ImguiCore",
		"EditorCore"
	})

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end

	group ""

filter {}