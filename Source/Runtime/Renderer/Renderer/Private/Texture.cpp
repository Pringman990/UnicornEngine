#include "pch.h"
#include "Texture.h"

#include<DDSTextureLoader11.h>

Texture2D::Texture2D()
	:
	mSRV(nullptr),
	mTexture2D(nullptr),
	mSize(Vector2(0,0))
{
}

Texture2D::~Texture2D()
{
	mSRV.Reset();
	mTexture2D.Reset();
}

void Texture2D::Bind(uint32 aSlot) const 
{
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void Texture2D::Resize(const Vector2& aNewSize)
{

	D3D11_TEXTURE2D_DESC oldTextureDesc = GetTexture2DDesc();
	oldTextureDesc.Width = static_cast<uint32>(aNewSize.x);
	oldTextureDesc.Height = static_cast<uint32>(aNewSize.y);

	D3D11_SHADER_RESOURCE_VIEW_DESC oldSrvDesc = {};
	mSRV->GetDesc(&oldSrvDesc);

	Release();

	if (!Create(this, oldTextureDesc, oldSrvDesc))
	{
		_LOG_RENDERER_ERROR("Failed to resize Texture2D");
	}

	mSize = aNewSize;
}

void Texture2D::Release()
{
	if (mSRV)
		mSRV.Reset();
	if (mTexture2D)
		mTexture2D.Reset();
}

Texture2D* Texture2D::Create(const std::wstring& aDDSPath)
{
	Texture2D* texture = new Texture2D();
	HRESULT result = DirectX::CreateDDSTextureFromFile(Renderer::GetInstance()->GetDevice(), aDDSPath.c_str(), reinterpret_cast<ID3D11Resource**>(texture->mTexture2D.GetAddressOf()), texture->mSRV.GetAddressOf());
	if (FAILED(result))
	{
		return nullptr;
	}
	return texture;
}

Texture2D* Texture2D::Create(const Vector2& aSize)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Texture2D> texture2D;
	ComPtr<ID3D11ShaderResourceView> textureView;

	//Setup Texture2D
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<uint32>(aSize.x);
	textureDesc.Height = static_cast<uint32>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D: {}", hr);
		return nullptr;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture2D.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return nullptr;
	}

	Texture2D* texture = new Texture2D();
	texture->mSize = aSize;
	texture->mMipLevel = textureDesc.MipLevels;
	texture->mSRV = textureView;
	texture->mTexture2D = texture2D;

	return texture;
}

bool Texture2D::Create(Texture2D* aTexture, D3D11_TEXTURE2D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Texture2D> texture2D;
	ComPtr<ID3D11ShaderResourceView> textureView;

	HRESULT hr = device->CreateTexture2D(&aTextureDesc, nullptr, texture2D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D: {}", hr);
		return false;
	}

	hr = device->CreateShaderResourceView(texture2D.Get(), &aSRVDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return false;
	}

	aTexture->mSRV = textureView;
	aTexture->mTexture2D = texture2D;

	return true;
}

Texture2D* Texture2D::Create(ID3D11Texture2D* aTexture)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	D3D11_TEXTURE2D_DESC textureDesc;
	aTexture->GetDesc(&textureDesc);

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = device->CreateShaderResourceView(aTexture, &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return nullptr;
	}

	Texture2D* texture = new Texture2D();
	texture->mSize = Vector2(static_cast<float>(textureDesc.Width), static_cast<float>(textureDesc.Height));
	texture->mMipLevel = textureDesc.MipLevels;
	texture->mSRV = textureView;
	texture->mTexture2D = aTexture;

	return texture;
}

ID3D11ShaderResourceView* Texture2D::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** Texture2D::GetAddressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11Texture2D* Texture2D::GetTexture2D()
{
	return mTexture2D.Get();
}

ID3D11Texture2D** Texture2D::GetAdressOfTexture2D()
{
	return mTexture2D.GetAddressOf();
}

D3D11_TEXTURE2D_DESC Texture2D::GetTexture2DDesc() const
{
	D3D11_TEXTURE2D_DESC desc = {};
	mTexture2D->GetDesc(&desc);

	return desc;
}