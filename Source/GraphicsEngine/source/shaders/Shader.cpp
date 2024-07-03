#include "GraphicsPch.h"
#include "Shader.h"

#include <fstream>
#include "source/DX11.h"
#include "ShaderManager.h"

#define LAYOUT_SIZE 6

Shader::Shader()
{
}

Shader::~Shader()
{
    mVertexShader.Reset();
    mPixelShader.Reset();
    mInputLayout.Reset();
}

bool Shader::CreateShader(
    const std::wstring& aVertexShaderFileName,
    const std::wstring& aPixelShaderFileName,
    const std::string& aVertexEntryPoint,
    const std::string& aVertexShaderModel,
    const std::string& aPixelEntryPoint,
    const std::string& aPixelShaderModel
)
{
    ShaderManager& shaderManager = Engine::GetGraphicsEngine().GetShaderManager();

    ID3DBlob* vertexBlob = nullptr;
    ID3D11VertexShader* vertexShader =  shaderManager.TryGetVertexShader(aVertexShaderFileName, vertexBlob, aVertexEntryPoint, aVertexShaderModel);
    if (vertexShader == nullptr)
    {
        std::wcout << "Failed to load Vertex Shader: " << aVertexShaderFileName.c_str() << std::endl;
        return false;
    }
    mVertexShader = vertexShader;

    ID3D11PixelShader* pixelShader = shaderManager.TryGetPixelShader(aPixelShaderFileName, aPixelEntryPoint, aPixelShaderModel);
    if (pixelShader == nullptr)
    {
        std::wcout << "Failed to load Pixel Shader: " << aPixelShaderFileName.c_str() << std::endl;
        return false;
    }

    mPixelShader = pixelShader;

    if (!Init())
    {
        std::cout << "Failed to Init Shader" << std::endl;
        return false;
    }

    if (mInputLayout)
        mInputLayout.Reset();

    if (!CreateInputLayout(vertexBlob))
    {
        D3D11_INPUT_ELEMENT_DESC layout[LAYOUT_SIZE] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        HRESULT result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreateInputLayout(layout, LAYOUT_SIZE, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &mInputLayout);
        if (FAILED(result))
        {
            _com_error err(result);
            LPCTSTR errorMessage = err.ErrorMessage();
            std::wcout << "Failed to create Shader: " << errorMessage << std::endl;
            return false;
        }
    }

    return true;
}

bool Shader::PrepareRender(D3D_PRIMITIVE_TOPOLOGY aPrimitiveTopology) const
{
    ID3D11DeviceContext* context = Engine::GetGraphicsEngine().GetDX11()->GetDeviceContext();
    if (!mVertexShader || !mPixelShader || !mInputLayout || !context)
    {
        std::cout << "Failed to render shader" << std::endl;
        return false;
    }

    context->IASetPrimitiveTopology(aPrimitiveTopology);
    context->IASetInputLayout(mInputLayout.Get());
    context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    return true;
}

bool Shader::CreateInputLayout(ID3DBlob* aVertexBlob)
{
    UNREFERENCED_PARAMETER(aVertexBlob);
    return false;
}

bool Shader::LoadVertexShader(const char* aPath, std::string& aData)
{
    std::ifstream vsFile;
    vsFile.open(aPath, std::ios::binary);
    aData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
    HRESULT result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreateVertexShader(aData.data(), aData.size(), nullptr, &mVertexShader);
    if (FAILED(result))
    {
        _com_error err(result);
        LPCTSTR errorMessage = err.ErrorMessage();
        std::wcout << "Failed to create Vertex Shader: " << errorMessage << std::endl;
        return false;
    }
    vsFile.close();
    return true;
}

bool Shader::LoadPixelShader(const char* aPath)
{
    std::ifstream psFile;
    psFile.open(aPath, std::ios::binary);
    std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    HRESULT result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &mPixelShader);
    if (FAILED(result))
    {
        _com_error err(result);
        LPCTSTR errorMessage = err.ErrorMessage();
        std::wcout << "Failed to create Pixel Shader: " << errorMessage << std::endl;
        return false;
    }
    psFile.close();
    return true;
}
