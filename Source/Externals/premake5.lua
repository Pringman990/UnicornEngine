
project "Externals"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	location (dirs.Local)
	targetdir (dirs.lib)

	disablewarnings { disabledwarnings }

	vpaths { ["*"] = {"**.*"}}
    externalanglebrackets "On"

	includedirs {
		".",
		"FBXImporter/FBXSDK/include",
		"FBXImporter/source",
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

		libdirs { "FBXImporter/FBXSDK/lib/debug" }
		links {
			"libfbxsdk",
		}

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "Full"

		libdirs { "FBXImporter/FBXSDK/lib/release" }
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