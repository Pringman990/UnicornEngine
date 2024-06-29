#include "GraphicsPch.h"
#include "Texture.h"

#include <DDSTextureLoader/DDSTextureLoader11.h>
#include "DX11.h"

Texture::Texture()
    :
    mHasBeenCreated(false),
    mDefaultMaterialTexturePath(L"../EngineAssets/Textures/defaultMaterial_c.dds")
{
}

Texture::~Texture()
{
    mSRV.Reset();
}

bool Texture::Create(const wchar_t* aPath)
{
    HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetInstance().GetDX11()->GetDevice(), aPath, nullptr, mSRV.GetAddressOf());
    if (FAILED(result))
    {
        result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetInstance().GetDX11()->GetDevice(), mDefaultMaterialTexturePath.c_str(), nullptr, mSRV.GetAddressOf());
       
        if (FAILED(result))
            return false;
    }

    mHasBeenCreated = true;
    return true;
}

void Texture::Bind(int aSlot)
{
    GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

Vector2 Texture::GetTextureSize() const
{
    ID3D11Resource* resource = nullptr;
    mSRV->GetResource(&resource);
    if (!resource)
    {
        return Vector2(0, 0);
    }
    ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>(resource);
    D3D11_TEXTURE2D_DESC desc;
    txt->GetDesc(&desc);

    Vector2 size(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
    resource->Release();

    return size;
}

ID3D11ShaderResourceView* Texture::GetSRV() const
{
    return mSRV.Get();
}
