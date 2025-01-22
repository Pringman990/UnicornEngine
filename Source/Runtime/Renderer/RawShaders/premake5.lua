print("Including Runtime/Renderer/RawShaders")

project "RawShaders"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	
	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.c",
		"**.hlsl",
		"**.hlsli"
	}

	includedirs {
		normalizePath(dirs.RawShaders) .. "/Private"
	}

	includeDependencies("RawShaders", 
	{
		dirs.RawShaders
	})

	vpaths { ["Public/*"] = {"Public/**.h", "Public/**.hpp", "Public/**.c", "Public/**.cpp", "Public/**.hlsl", "Public/**.hlsli"} }
	vpaths { ["Private/*"] = {"Private/**.h", "Private/**.hpp", "Private/**.c", "Private/**.cpp", "Private/**.hlsl", "Private/**.hlsli"}}

	shadermodel("5.0")

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(UCE_COMPILED_SHADERS_DIR.."/%{file.basename}"..".cso")

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