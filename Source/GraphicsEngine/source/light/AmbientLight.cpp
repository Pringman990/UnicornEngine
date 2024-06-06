#include "GraphicsPch.h"
#include "AmbientLight.h"

#include <DDSTextureLoader/DDSTextureLoader11.h>
#include "../DX11.h"

AmbientLight::AmbientLight(const Color& aColor, float aIntensity)
    :
    Light({}, aColor, aIntensity),
    mCubemap(nullptr),
    mNumberOfMips(0)
{
}

AmbientLight::~AmbientLight()
{
    mCubemap.Reset();
}

void AmbientLight::SetCubeMap(const std::wstring& aPath)
{
    HRESULT result = DirectX::CreateDDSTextureFromFile(GraphicsEngine::GetInstance().GetDX11()->GetDevice(), aPath.c_str(), nullptr, &mCubemap);
    assert(!FAILED(result));

    ID3D11Resource* theResource;
    mCubemap->GetResource(&theResource);
    ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(theResource);
    D3D11_TEXTURE2D_DESC cubeDescription;
    cubeTexture->GetDesc(&cubeDescription);

    mNumberOfMips = cubeDescription.MipLevels;
}

ComPtr<ID3D11ShaderResourceView> AmbientLight::GetCubeMap()
{
    return mCubemap;
}

uint32_t AmbientLight::GetNumMips() const
{
    return mNumberOfMips;
}
