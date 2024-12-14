#include "pch.h"
#include "DX11ShaderFactory.h"

#include "DX11ShaderTypes.h"
#include "DX11RenderingBackend.h"

DX11ShaderFactory::DX11ShaderFactory()
{
}

DX11ShaderFactory::~DX11ShaderFactory()
{
}

Shader* DX11ShaderFactory::CreateVertexShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{

    ComPtr<ID3DBlob> vertexBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(StringToWString(aFileName).c_str(), aEntryPoint.c_str(), aModel.c_str(), &vertexBlob);
    if (FAILED(hr))
    {
        return nullptr;
    }

    ID3D11VertexShader* vertexShader = nullptr;
    hr = DX11RenderingBackend::_GetInstance()->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        _LOG_RENDERER_ERROR("Failed to create vertex shader");
        return nullptr;
    }

    DX11VertexShaderBackend* vertexShaderBackend = new DX11VertexShaderBackend(
        vertexShader,
        vertexBlob
    );

    return vertexShaderBackend;
}

Shader* DX11ShaderFactory::CreatePixelShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{
    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(StringToWString(aFileName).c_str(), aEntryPoint.c_str(), aModel.c_str(), &shaderBlob);
    if (FAILED(hr))
    {
        return nullptr;
    }
    
    ID3D11PixelShader* pShader = nullptr;
    hr = DX11RenderingBackend::_GetInstance()->GetDevice()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pShader);
    if (FAILED(hr))
    {
        _LOG_RENDERER_ERROR("Failed to create pixel shader");
        shaderBlob->Release();
        return nullptr;
    }

    DX11PixelShaderBackend* pixelShaderBackend = new DX11PixelShaderBackend(pShader);

    shaderBlob->Release();

    return pixelShaderBackend;
}

HRESULT DX11ShaderFactory::CompileShaderFromFile(LPCWSTR aSrcFile, LPCSTR aEntryPoint, LPCSTR aShaderModel, ID3DBlob** ppBlobOut)
{
    DWORD shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    shaderflags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(aSrcFile, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, aEntryPoint, aShaderModel, shaderflags, 0, ppBlobOut, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::string errorMsg = (char*)errorBlob->GetBufferPointer();
            std::wstring srcFileStr = aSrcFile;
            _LOG_RENDERER_ERROR("Failed to compile shader: {}, Message: {}", WStringToString(srcFileStr), errorMsg);
            errorBlob->Release();
        }
        return hr;
    }
    if (errorBlob)
        errorBlob->Release();

    return S_OK;
}
