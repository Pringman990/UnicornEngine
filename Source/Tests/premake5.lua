--Core Root Directories
print("Including Tests")

group "Tests"
project "Tests"
	language "C++"
	cppdialect "C++20"
	kind "ConsoleApp"

	targetname("%{prj.name}")
    targetdir (UCE_EXECUTABLE_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		dirs.Tests
	}

	includeDependencies("Tests", 
	{
		dirs.Tests,
		"GoogleTest",
		"EntryPoint"
	})

	linkDependencies("Tests", 
	{
		"GoogleTest",
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	defines{
	}

group ""

filter {}