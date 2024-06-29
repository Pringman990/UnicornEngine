#include "GraphicsPch.h"
#include "ShaderManager.h"

#include "../DX11.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

ID3D11VertexShader* ShaderManager::TryGetVertexShader(const std::wstring& aShaderFileName, ID3DBlob*& aBlob, const std::string& aEntryPoint, const std::string& aShaderModel)
{
    std::wstring shaderFilePath = L"../Source/Shaders/source/" + aShaderFileName;
    std::wstring key = aShaderFileName + L"|" + std::wstring(aEntryPoint.begin(), aEntryPoint.end()) + L"|" + std::wstring(aShaderModel.begin(), aShaderModel.end());
    auto it = mCompiledVertexShaders.find(key);
    if (it != mCompiledVertexShaders.end())
    {
        aBlob = it->second.first;
        return it->second.second;
    }

    HRESULT hr = CompileShaderFromFile(shaderFilePath.c_str(), aEntryPoint.c_str(), aShaderModel.c_str(), &aBlob);
    if (FAILED(hr))
    {
        return nullptr;
    }

    ID3D11VertexShader* vShader = nullptr;
    hr = GraphicsEngine::GetInstance().GetDX11()->GetDevice()->CreateVertexShader(aBlob->GetBufferPointer(), aBlob->GetBufferSize(), nullptr, &vShader);
    if (FAILED(hr))
    {
        return nullptr;
    }

    mCompiledVertexShaders[key].second = vShader;
    mCompiledVertexShaders[key].first = aBlob;

    return mCompiledVertexShaders[key].second;
}

ID3D11PixelShader* ShaderManager::TryGetPixelShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint, const std::string& aShaderModel)
{
    std::wstring shaderFilePath = L"../Source/Shaders/source/" + aShaderFileName;
    std::wstring key = aShaderFileName + L"|" + std::wstring(aEntryPoint.begin(), aEntryPoint.end()) + L"|" + std::wstring(aShaderModel.begin(), aShaderModel.end());
    auto it = mCompiledPixelShaders.find(key);
    if (it != mCompiledPixelShaders.end())
        return it->second;

    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(shaderFilePath.c_str(), aEntryPoint.c_str(), aShaderModel.c_str(), &shaderBlob);
    if (FAILED(hr))
    {
        return nullptr;
    }

    ID3D11PixelShader* pShader = nullptr;
    hr = GraphicsEngine::GetInstance().GetDX11()->GetDevice()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pShader);
    if (FAILED(hr))
    {
        shaderBlob->Release();
        return nullptr;
    }

    mCompiledPixelShaders[key] = pShader;
    shaderBlob->Release();

    return mCompiledPixelShaders[key];
}

bool ShaderManager::RecompileVertexShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint, const std::string& aShaderModel)
{
    std::wstring key = aShaderFileName + L"|" + std::wstring(aEntryPoint.begin(), aEntryPoint.end()) + L"|" + std::wstring(aShaderModel.begin(), aShaderModel.end());
    auto it = mCompiledVertexShaders.find(key);
    if (it == mCompiledVertexShaders.end())
        return false;

    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(aShaderFileName.c_str(), aEntryPoint.c_str(), aShaderModel.c_str(), &shaderBlob);
    if (FAILED(hr))
    {
        return false;
    }

    ID3D11VertexShader* pShader = mCompiledVertexShaders[key].second;
    hr = GraphicsEngine::GetInstance().GetDX11()->GetDevice()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pShader);
    if (FAILED(hr))
    {
        shaderBlob->Release();
        return false;
    }

    mCompiledVertexShaders[key].first = shaderBlob;
    //shaderBlob->Release();

    return true;
}

bool ShaderManager::RecompilePixelShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint, const std::string& aShaderModel)
{
    std::wstring key = aShaderFileName + L"|" + std::wstring(aEntryPoint.begin(), aEntryPoint.end()) + L"|" + std::wstring(aShaderModel.begin(), aShaderModel.end());
    auto it = mCompiledPixelShaders.find(key);
    if (it == mCompiledPixelShaders.end())
        return false;

    ID3DBlob* shaderBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(aShaderFileName.c_str(), aEntryPoint.c_str(), aShaderModel.c_str(), &shaderBlob);
    if (FAILED(hr))
    {
        return false;
    }

    ID3D11PixelShader* pShader = mCompiledPixelShaders[key];
    hr = GraphicsEngine::GetInstance().GetDX11()->GetDevice()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pShader);
    if (FAILED(hr))
    {
        shaderBlob->Release();
        return false;
    }

    shaderBlob->Release();

    return true;
}

HRESULT ShaderManager::CompileShaderFromFile(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
    DWORD shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    shaderflags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(srcFile, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel, shaderflags, 0, ppBlobOut, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::string errorMsg = (char*)errorBlob->GetBufferPointer();
            std::cerr << "Shader compilation error: " << errorMsg << std::endl;
            OutputDebugStringA(errorMsg.c_str());
            errorBlob->Release();
        }
        return hr;
    }
    if (errorBlob)
        errorBlob->Release();

    return S_OK;
}
