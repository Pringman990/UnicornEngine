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

	shadermodel("6.0")

    defines{
        "VULKAN"
    }

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(UCE_COMPILED_SHADERS_DIR.."/%{file.basename}"..".spv")

    filter("files:**PS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Pixel")
        buildmessage("Compiling Pixel Shader %{file.basename}")
        buildcommands {
            "\"../../Source/ThirdParty/DXC/bin/x64/dxc.exe\"  -T ps_6_0 -spirv -Fo "..UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv %{file.relpath}"
        }
        buildoutputs {
            UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv"
        }
    
    filter("files:**VS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Vertex")
        buildmessage("Compiling Vertex Shader %{file.basename}")
        buildcommands {
            "\"../../Source/ThirdParty/DXC/bin/x64/dxc.exe\"  -T vs_6_0 -spirv -Fo "..UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv %{file.relpath}"
        }
        buildoutputs {
            UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv"
        }
    
    filter("files:**GS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Geometry")
        buildmessage("Compiling Geometry Shader %{file.basename}")
        buildcommands {
            "\"../../Source/ThirdParty/DXC/bin/x64/dxc.exe\" -T gs_6_0 -spirv -Fo "..UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv %{file.relpath}"
        }
        buildoutputs {
            UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv"
        }
    
    filter("files:**CS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Compute")
        buildmessage("Compiling Compute Shader %{file.basename}")
        buildcommands {
            "\"../../Source/ThirdParty/DXC/bin/x64/dxc.exe\"  -T cs_6_0 -spirv -Fo "..UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv %{file.relpath}"
        }
        buildoutputs {
            UCE_COMPILED_SHADERS_DIR.."/%{file.basename}.spv"
        }