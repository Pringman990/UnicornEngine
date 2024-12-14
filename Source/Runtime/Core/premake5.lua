--Core Root Directories
print("Including Runtime/Core/")

--Layer Zero
dirs["StandardTypes"]				= os.realpath(dirs.Core .. "StandardTypes/Public/")

--Layer One
dirs["MemoryDebugger"]				= os.realpath(dirs.Core .. "MemoryDebugger/Public/")
dirs["UniqueID"]					= os.realpath(dirs.Core .. "UniqueID/Public/")
dirs["Singleton"]					= os.realpath(dirs.Core .. "Singleton/Public/")

include (normalizePath(dirs.Singleton))

--Layer Two
dirs["Math"]						= os.realpath(dirs.Core .. "Math/Public/")
dirs["Timer"]						= os.realpath(dirs.Core .. "Timer/Public/")

--Layer Three
dirs["Logger"]						= os.realpath(dirs.Core .. "Logger/Public/")
dirs["EventDispatcher"]				= os.realpath(dirs.Core .. "EventDispatcher/Public/")
dirs["CrashHandler"]				= os.realpath(dirs.Core .. "CrashHandler/Public/")

--Layer Four
dirs["ApplicationCore"]				= os.realpath(dirs.Core .. "ApplicationCore/Public/")

--Layer Five
dirs["Input"]						= os.realpath(dirs.Core .. "Input/Public/")

dirs["Reflection"]					= os.realpath(dirs.Core .. "Reflection/Public/")
dirs["Utility"]						= os.realpath(dirs.Core .. "Utility/Public/")

project "Core"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		"Private/",
		normalizePath(dirs.StandardTypes) .. "/Private",
		normalizePath(dirs.MemoryDebugger) .. "/Private",
		normalizePath(dirs.UniqueID) .. "/Private",
		normalizePath(dirs.Math) .. "/Private",
		normalizePath(dirs.Timer) .. "/Private",
		normalizePath(dirs.Logger) .. "/Private",
		normalizePath(dirs.EventDispatcher) .. "/Private",
		normalizePath(dirs.CrashHandler) .. "/Private",
		normalizePath(dirs.ApplicationCore) .. "/Private",
		normalizePath(dirs.Input) .. "/Private",
		normalizePath(dirs.Reflection) .. "/Private",
		normalizePath(dirs.Utility) .. "/Private",
	}

	includeDependencies("Core", 
	{
		dirs.StandardTypes,
		dirs.MemoryDebugger,
		dirs.UniqueID,
		dirs.Math,
		dirs.Timer,
		dirs.Logger,
		dirs.EventDispatcher,
		dirs.CrashHandler,
		dirs.ApplicationCore,
		dirs.Input,
		dirs.Reflection,
		dirs.Utility,
		"SimpleMath",
		"Singleton",
		"Spdlog",
		"StackWalker"
	})

	linkDependencies("Core", 
	{
		"SimpleMath",
		"Singleton",
		"Spdlog",
		"StackWalker"
	})

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	pchheader "pch.h"
	pchsource "Private/pch.cpp"
	forceincludes { "pch.h" }

	if not os.isfile("Private/pch.h") then
        io.writefile("Private/pch.h", 
        "#pragma once\n" .. 
        "#pragma message(\"pch Core!\")\n\n"
        )
    end
   
    if not os.isfile("Private/pch.cpp") then
        io.writefile("Private/pch.cpp", 
        "#include \"pch.h\"")
    end

	--vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	--vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}