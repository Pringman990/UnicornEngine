#include "pch.h"
#include "TextureCube.h"

#include <DDSTextureLoader11.h>

TextureCube::TextureCube()
	:
	mSRV(nullptr),
	mTextureCube(nullptr)
{
}

TextureCube::~TextureCube()
{
	mSRV.Reset();
	mTextureCube.Reset();
}

void TextureCube::BindPS(uint32 aSlot) const
{
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void TextureCube::BindCS(uint32 aSlot) const
{
	Renderer::GetInstance()->GetDeviceContext()->CSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void TextureCube::UnbindCS(uint32 aSlot) const
{
	ID3D11ShaderResourceView* srv = nullptr;
	Renderer::GetInstance()->GetDeviceContext()->CSSetShaderResources(aSlot, 1, &srv);
}

ID3D11ShaderResourceView* TextureCube::GetSRV()
{
	return mSRV.Get();
}

TextureCube* TextureCube::Create(const std::string& aPath)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Resource> textureResource;
	ComPtr<ID3D11ShaderResourceView> textureView;

	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringToWString(aPath).c_str(), textureResource.GetAddressOf(), textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("DDS texture failed to create");
		return nullptr;
	}

	ComPtr<ID3D11Texture2D> texture2D;
	hr = textureResource.As(&texture2D);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to get texture2d from resource");
		return nullptr;
	}

	TextureCube* texture = new TextureCube();
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		textureView->GetDesc(&srvDesc);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		texture2D->GetDesc(&textureDesc);

		if (srvDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURECUBE)
		{
			texture->mTextureCube = texture2D;
			texture->mSize.x = static_cast<float>(textureDesc.Width);
			texture->mSize.y = static_cast<float>(textureDesc.Height);
			texture->mMipLevel = textureDesc.MipLevels;
			texture->mSRV = textureView;
		}
		else
		{
			_LOG_RENDERER_ERROR("Texture type not a cube");
			textureResource.Reset();
			textureView.Reset();
			return nullptr;
		}
	}

	return texture;
}
