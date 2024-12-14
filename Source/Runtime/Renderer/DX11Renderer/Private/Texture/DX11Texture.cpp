#include "pch.h"
#include "DX11Texture.h"

#include "DX11TextureFactory.h"

DX11Texture::DX11Texture()
	:
	mSRV(nullptr),
	mTexture2D(nullptr)
{
}

DX11Texture::~DX11Texture()
{
	mSRV.Reset();
	mTexture2D.Reset();
}

void DX11Texture::Bind()
{
}

void DX11Texture::Resize(const Vector2& aNewSize)
{

	D3D11_TEXTURE2D_DESC oldTextureDesc = GetTexture2DDesc();
	oldTextureDesc.Width = static_cast<uint32>(aNewSize.x);
	oldTextureDesc.Height = static_cast<uint32>(aNewSize.y);

	D3D11_SHADER_RESOURCE_VIEW_DESC oldSrvDesc = {};
	mSRV->GetDesc(&oldSrvDesc);

	Release();

	if (!DX11TextureFactory::CreateTexture(this, oldTextureDesc, oldSrvDesc))
	{
		_LOG_RENDERER_ERROR("Failed to resize DX11Texture");
	}

	mSize = aNewSize;
}

void DX11Texture::Release()
{
	if (mSRV)
		mSRV.Reset();
	if (mTexture2D)
		mTexture2D.Reset();
}

void* DX11Texture::GetUnderlyingSRV()
{
	return static_cast<void*>(mSRV.Get());
}

ID3D11ShaderResourceView* DX11Texture::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** DX11Texture::GetAddressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11Texture2D* DX11Texture::GetTexture2D()
{
	return mTexture2D.Get();
}

ID3D11Texture2D** DX11Texture::GetAdressOfTexture2D()
{
	return mTexture2D.GetAddressOf();
}

D3D11_TEXTURE2D_DESC DX11Texture::GetTexture2DDesc() const
{
	D3D11_TEXTURE2D_DESC desc = {};
	mTexture2D->GetDesc(&desc);

	return desc;
}
