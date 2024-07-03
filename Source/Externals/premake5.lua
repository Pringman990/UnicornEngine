
project "Externals"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	location (dirs.Local)
	targetdir (dirs.temp)

	disablewarnings { disabledwarnings }

	vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

	includedirs {
		".",
		"FBXImporter/FBXSDK/include",
		"FBXImporter/source",
		"yaml-cpp",
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c"
	}

	libdirs { dirs.lib }

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

		libdirs { 
			"FBXImporter/FBXSDK/lib/debug",
			dirs.lib .. "Debug/yaml-cppd.lib",
		}
		links {
			"libfbxsdk"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"        
		defines {"_RELEASE", "NDEBUG"}

		libdirs { 
			"FBXImporter/FBXSDK/lib/release", 
			dirs.lib .. "Release/yaml-cpp.lib",
		}
		links {
			"libfbxsdk",
		}

		filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"

		flags { 
			"MultiProcessorCompile"
		}
		
		links {
			"DXGI",
			"dxguid",
		}

		defines{
			"FBXSDK_SHARED"
		}