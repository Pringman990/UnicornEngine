--Core Root Directories
print("Including Runtime/Core/")

--Layer One
dirs["MemoryDebugger"]				= os.realpath(dirs.Core .. "MemoryDebugger/Public/")

include (normalizePath(dirs.MemoryDebugger))

--Layer Two
dirs["Math"]						= os.realpath(dirs.Core .. "Math/Public/")
dirs["STLWrapper"]					= os.realpath(dirs.Core .. "STLWrapper/Public/")
dirs["Timer"]						= os.realpath(dirs.Core .. "Timer/Public/")

include (normalizePath(dirs.Math))
include (normalizePath(dirs.STLWrapper))
include (normalizePath(dirs.Timer))

--Layer Three
dirs["Logger"]						= os.realpath(dirs.Core .. "Logger/Public/")
dirs["EventDispatcher"]				= os.realpath(dirs.Core .. "EventDispatcher/Public/")
dirs["CrashHandler"]				= os.realpath(dirs.Core .. "CrashHandler/Public/")

include (normalizePath(dirs.Logger))
include (normalizePath(dirs.EventDispatcher))
include (normalizePath(dirs.CrashHandler))

--Layer Four
dirs["ApplicationCore"]				= os.realpath(dirs.Core .. "ApplicationCore/Public/")

include (normalizePath(dirs.ApplicationCore))

--Layer Five
dirs["Input"]						= os.realpath(dirs.Core .. "Input/Public/")

include (normalizePath(dirs.Input))

project "Core"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(defaultTargetName)
    targetdir (defaultTargetDir)

    objdir(defaultObjDir)
    location (defaultLocationDir)

	includedirs {
		--Layer One
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
		
		--Layer Two
		inheritAndIncludeDirsFromProject("Math"),
		inheritAndIncludeDirsFromProject("STLWrapper"),
		inheritAndIncludeDirsFromProject("Timer"),
		
		--Layer Three
		inheritAndIncludeDirsFromProject("Logger"),
		inheritAndIncludeDirsFromProject("EventDispatcher"),
		inheritAndIncludeDirsFromProject("CrashHandler"),
		
		--Layer Four
		inheritAndIncludeDirsFromProject("ApplicationCore"),

		--Layer Five
		inheritAndIncludeDirsFromProject("Input"),
	}

	projectInheritDirs["Core"] = flattenTable({
		--Layer One
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
		
		--Layer Two
		inheritAndIncludeDirsFromProject("Math"),
		inheritAndIncludeDirsFromProject("STLWrapper"),
		inheritAndIncludeDirsFromProject("Timer"),
		
		--Layer Three
		inheritAndIncludeDirsFromProject("Logger"),
		inheritAndIncludeDirsFromProject("EventDispatcher"),
		inheritAndIncludeDirsFromProject("CrashHandler"),
		
		--Layer Four
		inheritAndIncludeDirsFromProject("ApplicationCore"),

		--Layer Five
		inheritAndIncludeDirsFromProject("Input"),
	})

	links{
		--Layer One
		"MemoryDebugger",

		--Layer Two
		"Math",
		"STLWrapper",
		"Timer",
		
		--Layer Three
		"Logger",
		"EventDispatcher",
		"CrashHandler",
		
		--Layer Four
		"ApplicationCore",

		--Layer Five
		"Input",
	}

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end