#include "pch.h"
#include "DX11Texture.h"

#include "Texture/DX11TextureFactory.h"

#pragma region Texture2D
DX11Texture2D::DX11Texture2D()
	:
	mSRV(nullptr),
	mTexture2D(nullptr)
{
}

DX11Texture2D::~DX11Texture2D()
{
	mSRV.Reset();
	mTexture2D.Reset();
}

void DX11Texture2D::Bind(uint32 aSlot) const 
{
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void DX11Texture2D::Resize(const Vector2& aNewSize)
{

	D3D11_TEXTURE2D_DESC oldTextureDesc = GetTexture2DDesc();
	oldTextureDesc.Width = static_cast<uint32>(aNewSize.x);
	oldTextureDesc.Height = static_cast<uint32>(aNewSize.y);

	D3D11_SHADER_RESOURCE_VIEW_DESC oldSrvDesc = {};
	mSRV->GetDesc(&oldSrvDesc);

	Release();

	if (!DX11TextureFactory::CreateTexture2D(this, oldTextureDesc, oldSrvDesc))
	{
		_LOG_RENDERER_ERROR("Failed to resize DX11Texture2D");
	}

	mSize = aNewSize;
}

void DX11Texture2D::Release()
{
	if (mSRV)
		mSRV.Reset();
	if (mTexture2D)
		mTexture2D.Reset();
}

void* DX11Texture2D::GetUnderlyingSRV()
{
	return static_cast<void*>(mSRV.Get());
}

ID3D11ShaderResourceView* DX11Texture2D::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** DX11Texture2D::GetAddressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11Texture2D* DX11Texture2D::GetTexture2D()
{
	return mTexture2D.Get();
}

ID3D11Texture2D** DX11Texture2D::GetAdressOfTexture2D()
{
	return mTexture2D.GetAddressOf();
}

D3D11_TEXTURE2D_DESC DX11Texture2D::GetTexture2DDesc() const
{
	D3D11_TEXTURE2D_DESC desc = {};
	mTexture2D->GetDesc(&desc);

	return desc;
}
#pragma endregion

#pragma region TextureCube
DX11TextureCube::DX11TextureCube()
	:
	mSRV(nullptr),
	mTextureCube(nullptr)
{
}

DX11TextureCube::~DX11TextureCube()
{
	mSRV.Reset();
	mTextureCube.Reset();
}

void DX11TextureCube::Bind(uint32 aSlot) const
{
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void* DX11TextureCube::GetUnderlyingSRV()
{
	return static_cast<void*>(mSRV.Get());
}

#pragma endregion