#include "InputLayoutManager.h"

#include "Renderer.h"
#include "ShaderManager.h"

#include <d3d11.h>
#include <d3dcompiler.h>

InputLayoutManager::InputLayoutManager(Renderer* InRenderer)
    :
    mRenderer(InRenderer)
{
}

InputLayoutManager::~InputLayoutManager()
{
}

InputLayout* InputLayoutManager::TryGetLayout(void* VsBlob, const VSReflectData& VsReflection)
{
    ID3DBlob* vsBlob = static_cast<ID3DBlob*>(VsBlob);

    ComPtr<ID3DBlob> signatureBlob;
    HRESULT hr =D3DGetInputSignatureBlob(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &signatureBlob
    );

    if (FAILED(hr))
    {
        LOG_ERROR("Failed to reflect input layout: {}", hr);
        return nullptr;
    }

    size_t hash = HashSignatureBlob(vsBlob);

    auto it = mLayouts.find(hash);
    if (it != mLayouts.end())
    {
        return &it->second;
    }

    InputLayout layout;

    D3D11_INPUT_ELEMENT_DESC layout2[] =
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
    D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
    D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };


    hr = mRenderer->GetLogicalDevice()->CreateInputLayout(
        layout2,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &layout.layout
    );

   /* hr = mRenderer->GetLogicalDevice()->CreateInputLayout(
        VsReflection.inputDescs.data(), 
        static_cast<uint32>(VsReflection.inputDescs.size()),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &layout.layout
    );*/

    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create input layout: {}", hr);
        return nullptr;
    }

    mLayouts[hash] = std::move(layout);

    return &mLayouts[hash];
}

size_t InputLayoutManager::HashSignatureBlob(ID3DBlob* Signature)
{
    const uint8_t* data = (const uint8_t*)Signature->GetBufferPointer();
    size_t len = Signature->GetBufferSize();
    size_t hash = 1469598103934665603ull;
   
    for (size_t i = 0; i < len; ++i)
        hash = (hash ^ data[i]) * 1099511628211ull;

    return hash;
}
