--Core Root Directories
print("Including Runtime/Core/")

--Layer Zero
dirs["StandardTypes"]				= os.realpath(dirs.Core .. "StandardTypes/Public/")

include (normalizePath(dirs.StandardTypes))

--Layer One
dirs["MemoryDebugger"]				= os.realpath(dirs.Core .. "MemoryDebugger/Public/")
dirs["UniqueID"]					= os.realpath(dirs.Core .. "UniqueID/Public/")

include (normalizePath(dirs.MemoryDebugger))
include (normalizePath(dirs.UniqueID))

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
		--Layer Zero
		inheritAndIncludeDirsFromProject("StandardTypes"),

		--Layer One
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
		inheritAndIncludeDirsFromProject("UniqueID"),
		
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
		--Layer Zero
		inheritAndIncludeDirsFromProject("StandardTypes"),

		--Layer One
		inheritAndIncludeDirsFromProject("MemoryDebugger"),
		inheritAndIncludeDirsFromProject("UniqueID"),
		
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
		--Layer Zero
		"StandardTypes",

		--Layer One
		"MemoryDebugger",
		"UniqueID",

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