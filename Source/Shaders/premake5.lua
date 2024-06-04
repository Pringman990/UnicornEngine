include "../../Premake/common.lua"

project "Shaders"
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	location (dirs.Local)
	targetdir (dirs.temp)
	implibdir (dirs.lib)

	files {
		"**.hlsl",
		"**.hlsli",
	}

	includedirs {
		".",
		dirs.shaders
	}


	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "Full"

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"

		flags { 
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}

	shadermodel("5.0")
	local shader_dir = dirs.shaders_output
	os.mkdir(shader_dir)

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(shader_dir.."/%{file.basename}"..".cso")

	filter("files:**FX.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Effect")

	filter("files:**PS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")

	filter("files:**VS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")

	filter("files:**GS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")
	
	filter("files:**HS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Hull")
	
	filter("files:**DS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Domain")

	filter("files:**CS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Compute")
		
	filter("files:**MS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Mesh")
	
	filter("files:**AS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Amplification")

	filter("files:**TE.hlsl")
	removeflags("ExcludeFromBuild")
	shadertype("Texture")

	filter("files:**RS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("RootSignature")

	-- Warnings as errors
	shaderoptions({"/WX"})