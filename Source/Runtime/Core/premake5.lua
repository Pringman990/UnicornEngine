--Core Root Directories
print("Including Runtime/Core/")

--Layer Zero
--dirs["StandardTypes"]				= os.realpath(dirs.Core .. "/StandardTypes")
--
----Layer One
--dirs["MemoryDebugger"]				= os.realpath(dirs.Core .. "/MemoryDebugger")
--dirs["UniqueID"]					= os.realpath(dirs.Core .. "/UniqueID")
--
----Layer Two
--dirs["Math"]						= os.realpath(dirs.Core .. "/Math")
--dirs["Timer"]						= os.realpath(dirs.Core .. "/Timer")
--
----Layer Three
--dirs["Logger"]						= os.realpath(dirs.Core .. "/Logger")
--dirs["EventDispatcher"]				= os.realpath(dirs.Core .. "/EventDispatcher")
--dirs["CrashHandler"]				= os.realpath(dirs.Core .. "/CrashHandler")
--dirs["FileWatcher"]					= os.realpath(dirs.Core .. "/FileWatcher")
--
----Layer Four
--dirs["Application"]				= os.realpath(dirs.Core .. "/Application")
--
----Layer Five
--dirs["Input"]						= os.realpath(dirs.Core .. "/Input")
--
--dirs["Reflection"]					= os.realpath(dirs.Core .. "/Reflection")
--dirs["Utility"]						= os.realpath(dirs.Core .. "/Utility")

project "Core"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	includedirs {
		normalizePath(dirs.Core) .. "/Private",
		--dirs.StandardTypes,
		--dirs.MemoryDebugger,
		--dirs.UniqueID,
		--dirs.Math,
		--dirs.Timer,
		--dirs.Logger,
		--dirs.EventDispatcher,
		--dirs.CrashHandler,
		--dirs.FileWatcher,
		--dirs.Application,
		--dirs.Input,
		--dirs.Reflection,
		--dirs.Utility,
	}

	includeDependencies("Core", 
	{
		dirs.Core,
		--dirs.StandardTypes,
		--dirs.MemoryDebugger,
		--dirs.UniqueID,
		--dirs.Math,
		--dirs.Timer,
		--dirs.Logger,
		--dirs.EventDispatcher,
		--dirs.CrashHandler,
		--dirs.FileWatcher,
		--dirs.ApplicationCore,
		--dirs.Input,
		--dirs.Reflection,
		--dirs.Utility,
		"SimpleMath",
		"Singleton",
		"Spdlog",
		"StackWalker",
		"Build"
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

	filter ("platforms:x64-sdl")
		links {"SDL2"}
		includedirs {
			os.getenv("VULKAN_SDK") .. "/Include/SDL2"
		}
		libdirs{
			os.getenv("VULKAN_SDK") .. "/Lib"
		}
	
	filter{}

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

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp"}}