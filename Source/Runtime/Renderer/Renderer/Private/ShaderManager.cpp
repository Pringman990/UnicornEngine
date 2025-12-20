#include "ShaderManager.h"

#include "Renderer.h"
#include "InputLayoutManager.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>


#define ENGINE_OWNED_CBUFFERS STRINGIFY(FrameConstantsData), STRINGIFY(CameraConstantsData), STRINGIFY(ObjectConstantBufferData)

ShaderManager::ShaderManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

ShaderManager::~ShaderManager()
{
}

GPUResourceHandle<ShaderProgram> ShaderManager::TryGetShaderProgram(const Path& SourcePath)
{
	auto it = mPathToShaderProgram.find(SourcePath);
	if (it != mPathToShaderProgram.end())
	{
		return it->second;
	}

	ComPtr<ID3DBlob> vertexBlob = nullptr;
	bool result = CompileShader(SourcePath, HLSL_VS_ENTRYPOINT, HLSL_VS_SUPPORTED_VERSION, vertexBlob);
	if (!result)
	{
		LOG_ERROR("Failed to compile vertex shader");
		return GPUResourceHandle<ShaderProgram>::Invalid();
	}

	ComPtr<ID3D11VertexShader> vertexShader = nullptr;
	HRESULT hr = mRenderer->GetLogicalDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create vertex shader");
		return GPUResourceHandle<ShaderProgram>::Invalid();
	}

	ComPtr<ID3DBlob> pixelBlob = nullptr;
	result = CompileShader(SourcePath, HLSL_FS_ENTRYPOINT, HLSL_FS_SUPPORTED_VERSION, pixelBlob);
	if (!result)
	{
		LOG_ERROR("Failed to compile pixel shader");
		return GPUResourceHandle<ShaderProgram>::Invalid();
	}

	ComPtr<ID3D11PixelShader> pixelShader = nullptr;
	hr = mRenderer->GetLogicalDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create pixel shader: {}", hr);
		return GPUResourceHandle<ShaderProgram>::Invalid();
	}

	GPUResourceHandle<ShaderProgram> handle = mShaderProgramPool.Allocate();
	ShaderProgram* shader = mShaderProgramPool.Get(handle);
	shader->fs.shader = pixelShader;
	shader->fs.bytecode = pixelBlob;
	shader->fs.bind = ShaderStageBind::FS;
	shader->vs.shader = vertexShader;
	shader->vs.bytecode = vertexBlob;
	shader->vs.bind = ShaderStageBind::VS;
	shader->vs.reflectedInfo = ReflectShader(vertexBlob);
	shader->fs.reflectedInfo = ReflectShader(pixelBlob);

	shader->inputLayout = mRenderer->GetInputManager()->TryGetLayout(vertexBlob.Get(), shader->vs.reflectedInfo);
	if (!shader->inputLayout)
	{
		mShaderProgramPool.Remove(handle);

		LOG_ERROR("Failed to create input layout");
		return GPUResourceHandle<ShaderProgram>::Invalid();
	}

	mPathToShaderProgram[SourcePath] = handle;

	return handle;
}

bool ShaderManager::CompileShader(const Path& VirtualPath, const String& EntryPoint, const String& ShaderModel, ComPtr<ID3DBlob>& Blob)
{
	FileSystem* fileSystem = FileSystem::Instance();
	Path path = fileSystem->GetAbsolutPath(VirtualPath);

	DWORD shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	shaderflags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3DBlob> shaderBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(StringToWString(path).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint.c_str(), ShaderModel.c_str(), shaderflags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			std::string errorMsg = (char*)errorBlob->GetBufferPointer();
			LOG_ERROR("Failed to compile shader: {}, Message: {}", path, errorMsg);
			errorBlob->Release();
		}
		return false;
	}
	if (errorBlob)
		errorBlob->Release();

	Blob = shaderBlob;

	return true;
}

ShaderReflectionInfo ShaderManager::ReflectShader(ComPtr<ID3DBlob>& BytecodeBlob)
{
	ComPtr<ID3D11ShaderReflection> reflector;
	HRESULT hr = D3DReflect(
		BytecodeBlob->GetBufferPointer(),
		BytecodeBlob->GetBufferSize(),
		IID_PPV_ARGS(&reflector)
	);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to reflect shader: {}", hr);
		return {};
	}

	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	ShaderReflectionInfo reflectionInfo{};

	reflectionInfo.inputParams.reserve(shaderDesc.InputParameters);

	//Vertex input parameters
	for (uint32 i = 0; i < shaderDesc.InputParameters; i++)
	{
		ShaderReflectionInfo::InputParam inputParam;

		D3D11_SIGNATURE_PARAMETER_DESC signatureParam;
		reflector->GetInputParameterDesc(i, &signatureParam);

		inputParam.semanticName = signatureParam.SemanticName;
		inputParam.semanticIndex = signatureParam.SemanticIndex;

		RenderFormat inputFormat = RenderFormat::UNKNOWN;
		switch (signatureParam.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			if (signatureParam.Mask == 0x1) //Single component (x)
			{
				inputFormat = RenderFormat::R32_UINT;
			}
			else if (signatureParam.Mask == 0x3) //Two components (xy)
			{
				inputFormat = RenderFormat::R32G32_UINT;
			}
			else if (signatureParam.Mask == 0x7) //Three components (xyz)
			{
				inputFormat = RenderFormat::R32G32B32_UINT;
			}
			else if (signatureParam.Mask == 0xF) //Four components (xyzw)
			{
				inputFormat = RenderFormat::R32G32B32A32_UINT;
			}
			break;
		}
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			if (signatureParam.Mask == 0x1) //Single component (x)
			{
				inputFormat = RenderFormat::R32_SINT;
			}
			else if (signatureParam.Mask == 0x3) //Two components (xy)
			{
				inputFormat = RenderFormat::R32G32_SINT;
			}
			else if (signatureParam.Mask == 0x7) //Three components (xyz)
			{
				inputFormat = RenderFormat::R32G32B32_SINT;
			}
			else if (signatureParam.Mask == 0xF) //Four components (xyzw)
			{
				inputFormat = RenderFormat::R32G32B32A32_SINT;
			}
			break;
		}
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			if (signatureParam.Mask == 0x1) //Single component (x)
			{
				inputFormat = RenderFormat::R32_FLOAT;
			}
			else if (signatureParam.Mask == 0x3) //Two components (xy)
			{
				inputFormat = RenderFormat::R32G32_FLOAT;
			}
			else if (signatureParam.Mask == 0x7) //Three components (xyz)
			{
				inputFormat = RenderFormat::R32G32B32_FLOAT;
			}
			else if (signatureParam.Mask == 0xF) //Four components (xyzw)
			{
				inputFormat = RenderFormat::R32G32B32A32_FLOAT;
			}
			break;
		}
		default:
			LOG_WARNING("Unknown shader input component type");
			break;
		}

		if (inputFormat == RenderFormat::UNKNOWN)
		{
			LOG_WARNING("Unknown shader input format");
		}

		inputParam.format = inputFormat;

		reflectionInfo.inputParams.emplace_back(inputParam);
	}

	reflectionInfo.boundResources.reserve(shaderDesc.BoundResources);
	//Textures and bound resources
	for (uint32 i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		reflector->GetResourceBindingDesc(i, &bindDesc);
		if (bindDesc.Type == D3D_SIT_CBUFFER)
		{
			continue;
		}

		ShaderReflectionInfo::BoundResources boundResource{};
		boundResource.name = bindDesc.Name;
		boundResource.bindPoint = bindDesc.BindPoint;
		boundResource.type = FromDXInputType(bindDesc.Type);
		if (boundResource.type == ShaderResourceType::Undefined)
		{
			LOG_WARNING("Shader reflected bound resource is unsupported type");
			continue;
		}

		reflectionInfo.boundResources.emplace_back(boundResource);
	}

	reflectionInfo.cBuffers.reserve(shaderDesc.ConstantBuffers);
	//Textures and bound resources
	for (uint32 i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		ID3D11ShaderReflectionConstantBuffer* reflectedBuffer = reflector->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC cBufferDes{};
		reflectedBuffer->GetDesc(&cBufferDes);

		if (!cBufferDes.Name)
		{
			LOG_WARNING("Shader reflection skipped unnamed constant buffer, please name the constant buffer");
			continue;
		}

		D3D11_SHADER_INPUT_BIND_DESC bindDesc{};
		reflector->GetResourceBindingDescByName(cBufferDes.Name, &bindDesc);

		ShaderReflectionInfo::CBuffer cBuffer{};
		cBuffer.name = cBufferDes.Name ? cBufferDes.Name : "";
		cBuffer.size = cBufferDes.Size;
		cBuffer.bindPoint = bindDesc.BindPoint;
		
		if(StartsWith(cBuffer.name, "Engine"))
			cBuffer.engineOwned = true;
		else
			cBuffer.engineOwned = false;

		cBuffer.variables.reserve(cBufferDes.Variables);
		for (uint32 j = 0; j < cBufferDes.Variables; j++)
		{
			ID3D11ShaderReflectionVariable* var = reflectedBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			ShaderReflectionInfo::CBuffer::Variable variable;

			variable.name = varDesc.Name;
			variable.size = varDesc.Size;
			variable.offset = varDesc.StartOffset;

			ID3D11ShaderReflectionType* type = var->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			variable.type = FromD3DShaderVariableType(typeDesc);

			cBuffer.variables.emplace_back(variable);
		}

		reflectionInfo.cBuffers.emplace_back(cBuffer);
	}

	return reflectionInfo;
}
