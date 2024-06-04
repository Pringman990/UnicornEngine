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
    HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetInstance().GetDX11().GetDevice(), aPath, nullptr, mSRV.GetAddressOf());
    if (FAILED(result))
    {
        result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetInstance().GetDX11().GetDevice(), mDefaultMaterialTexturePath.c_str(), nullptr, mSRV.GetAddressOf());
       
        if (FAILED(result))
            return false;
    }

    mHasBeenCreated = true;
    return true;
}

void Texture::Bind(int aSlot)
{
    GraphicsEngine::GetInstance().GetDX11().GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

ID3D11ShaderResourceView* Texture::GetSRV() const
{
    return mSRV.Get();
}
