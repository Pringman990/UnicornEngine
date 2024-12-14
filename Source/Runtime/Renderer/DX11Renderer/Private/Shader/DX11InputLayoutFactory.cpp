#include "pch.h"
#include "DX11InputLayoutFactory.h"

#include "DX11InputLayout.h"
#include "DX11VertexShaderBackend.h"

DX11InputLayoutFactory::DX11InputLayoutFactory()
{
}

DX11InputLayoutFactory::~DX11InputLayoutFactory()
{

}

InputLayout* DX11InputLayoutFactory::GetModelLayout(Shader* aVertexShader)
{
    DX11InputLayout* inputLayout = new DX11InputLayout(eInputLayoutStandardTypes::eModel);

    constexpr int8_t layoutSize = 6;
    D3D11_INPUT_ELEMENT_DESC layout[layoutSize] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    DX11VertexShaderBackend* dx11VertexShader = static_cast<DX11VertexShaderBackend*>(aVertexShader);
    
    HRESULT result = DX11RenderingBackend::_GetInstance()->GetDevice()->CreateInputLayout(layout, layoutSize, dx11VertexShader->GetVertexBlob()->GetBufferPointer(), dx11VertexShader->GetVertexBlob()->GetBufferSize(), &inputLayout->mLayout);
    if (FAILED(result) || inputLayout == nullptr)
    {
        _com_error err(result);
        std::wstring errorMessage = err.ErrorMessage();
        std::wstring str = L"Failed to create model input layout DX11, Error: " + errorMessage;
        _ENSURE_RENDERER(false, WStringToString(str));

        return nullptr;
    }

    return inputLayout;
}