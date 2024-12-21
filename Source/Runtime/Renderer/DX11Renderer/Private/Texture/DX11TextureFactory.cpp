#include "pch.h"
#include "Texture/DX11TextureFactory.h"

#include "DX11Texture.h"
#include <DDSTextureLoader11.h>

DX11TextureFactory::DX11TextureFactory()
{
}

DX11TextureFactory::~DX11TextureFactory()
{
}

Texture* DX11TextureFactory::CreateTexture(const std::string& aPath)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

	ComPtr<ID3D11Resource> textureResource;
	ComPtr<ID3D11ShaderResourceView> textureView;

	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringToWString(aPath).c_str(), textureResource.GetAddressOf(), textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("DDS dx11 texture failed to create");
		return nullptr;
	}

	ComPtr<ID3D11Texture2D> texture2D;
	hr = textureResource.As(&texture2D);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to get texture2d from resource");
		return nullptr;
	}

	eTextureType textureType = eTextureType::eInvalid;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		textureView->GetDesc(&srvDesc);

		if (srvDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURECUBE)
		{
			textureType = eTextureType::eTextureCube;
		}
		else if (srvDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
		{
			textureType = eTextureType::eTexture2D;
		}
		else
		{
			_LOG_RENDERER_ERROR("Texture type not supported");
			textureResource.Reset();
			textureView.Reset();
			return nullptr;
		}
	}

	DX11Texture* dx11Texture = new DX11Texture();
	dx11Texture->mTexture2D = texture2D;

	dx11Texture->mType = textureType;

	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		texture2D->GetDesc(&textureDesc);

		dx11Texture->mSize.x = static_cast<float>(textureDesc.Width);
		dx11Texture->mSize.y = static_cast<float>(textureDesc.Height);
		dx11Texture->mMipLevel = textureDesc.MipLevels;
		dx11Texture->mSRV = textureView;
	}

	return dx11Texture;
}

DX11Texture* DX11TextureFactory::CreateTexture(const Vector2& aSize)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

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

	DX11Texture* dx11Texture = new DX11Texture();
	dx11Texture->mSize = aSize;
	dx11Texture->mMipLevel = textureDesc.MipLevels;
	dx11Texture->mSRV = textureView;
	dx11Texture->mTexture2D = texture2D;
	dx11Texture->mType = eTextureType::eTexture2D;
	dx11Texture->mTextureID = UniqueID::InvalidID;

	return dx11Texture;
}

bool DX11TextureFactory::CreateTexture(DX11Texture* aTexture, D3D11_TEXTURE2D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

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

DX11Texture* DX11TextureFactory::CreateTexture(ID3D11Texture2D* aTexture)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

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

	DX11Texture* dx11Texture = new DX11Texture();
	dx11Texture->mSize = Vector2(static_cast<float>(textureDesc.Width), static_cast<float>(textureDesc.Height));
	dx11Texture->mMipLevel = textureDesc.MipLevels;
	dx11Texture->mSRV = textureView;
	dx11Texture->mTexture2D = aTexture;
	dx11Texture->mType = eTextureType::eTexture2D;
	dx11Texture->mTextureID = UniqueID::InvalidID;

	return dx11Texture;
}
