#include "ShaderManager.h"

#include "Renderer.h"
#include "InputLayoutManager.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>

ShaderManager::ShaderManager(Renderer* InRenderer)
    :
    mRenderer(InRenderer)
{
}

ShaderManager::~ShaderManager()
{
}

GPUResourceHandle<Shader> ShaderManager::CreateShader(const String& VertexPath, const String& FragmentPath)
{
    ComPtr<ID3DBlob> vertexBlob = nullptr;
    HRESULT hr = CompileShader(VertexPath, "vs_5_0", &vertexBlob);
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to compile vertex shader: {}", hr);
        return GPUResourceHandle<Shader>::Invalid();
    }

    ID3D11VertexShader* vertexShader = nullptr;
    hr = mRenderer->GetLogicalDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create vertex shader");
        return GPUResourceHandle<Shader>::Invalid();
    }

    ComPtr<ID3DBlob> pixelBlob = nullptr;
    hr = CompileShader(FragmentPath, "ps_5_0", &pixelBlob);
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to compile pixel shader: {}", hr);
        return GPUResourceHandle<Shader>::Invalid();
    }

    ID3D11PixelShader* pixelShader = nullptr;
    hr = mRenderer->GetLogicalDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create pixel shader: {}", hr);
        return GPUResourceHandle<Shader>::Invalid();
    }

    GPUResourceHandle<Shader> handle = mShaderPool.Allocate();
    Shader* shader = mShaderPool.Get(handle);
    shader->ps = pixelShader;
    shader->vs = vertexShader;
    shader->layout = mRenderer->GetInputManager()->TryGetLayout(vertexBlob.Get(), ReflectVSShader(vertexBlob.Get()));

	return handle;
}

bool ShaderManager::CompileShader(const String& VirtualPath, const String& ShaderModel, void** Blob)
{
    FileSystem* fileSystem = SubsystemManager::Get<FileSystem>();
    String path = fileSystem->GetAbsolutPath(VirtualPath);
    
    DWORD shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    shaderflags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

    ID3DBlob* errorBlob = nullptr;
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(StringToWString(path).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", ShaderModel.c_str(), shaderflags, 0, &shaderBlob, &errorBlob);
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

    *Blob = shaderBlob;

    return true;
}

VSReflectData ShaderManager::ReflectVSShader(ID3DBlob* Blob)
{
    ComPtr<ID3D11ShaderReflection> reflector;
    HRESULT hr = D3DReflect(
        Blob->GetBufferPointer(), 
        Blob->GetBufferSize(),
        IID_PPV_ARGS(&reflector)
    );

    if (FAILED(hr))
    {
        LOG_ERROR("Failed to reflect vertex shader: {}", hr);
        return {};
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    VSReflectData reflectedData;
    reflectedData.inputDescs.reserve(shaderDesc.InputParameters);

    for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param;
        reflector->GetInputParameterDesc(i, &param);

        D3D11_INPUT_ELEMENT_DESC desc = {};
        desc.SemanticName = param.SemanticName;
        desc.SemanticIndex = param.SemanticIndex;
        desc.InputSlot = 0;
        desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        if (param.Mask == 1)
            desc.Format = DXGI_FORMAT_R32_FLOAT;
        else if (param.Mask <= 3)
            desc.Format = DXGI_FORMAT_R32G32_FLOAT;
        else if (param.Mask <= 7)
            desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        else if (param.Mask <= 15)
            desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        reflectedData.inputDescs.push_back(desc);
    }

	return reflectedData;
}
