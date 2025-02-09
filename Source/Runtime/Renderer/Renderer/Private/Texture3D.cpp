#include "pch.h"
#include "Texture3D.h"

Texture3D::Texture3D()
	:
	mSRV(nullptr),
	mTexture3D(nullptr),
	mSize(Vector2(0, 0)),
	mMipLevel(0)
{
}

Texture3D::~Texture3D()
{
	mSRV.Reset();
	mTexture3D.Reset();
}

void Texture3D::BindPS(uint32 aSlot) const
{
	Renderer::GetInstance()->GetDeviceContext()->PSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void Texture3D::BindCS(uint32 aSlot) const
{
	Renderer::GetInstance()->GetDeviceContext()->CSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}

void Texture3D::UnbindCS(uint32 aSlot) const
{
	ID3D11ShaderResourceView* srv = nullptr;
	Renderer::GetInstance()->GetDeviceContext()->CSSetShaderResources(aSlot, 1, &srv);
}

void Texture3D::Resize(const Vector3& aNewSize)
{
	D3D11_TEXTURE3D_DESC oldTextureDesc = GetTexture3DDesc();
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

void Texture3D::Release()
{
	if (mSRV)
		mSRV.Reset();
	if (mTexture3D)
		mTexture3D.Reset();
}

bool Texture3D::Create(Texture3D* aTexture, D3D11_TEXTURE3D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Texture3D> texture3D;
	ComPtr<ID3D11ShaderResourceView> textureView;

	HRESULT hr = device->CreateTexture3D(&aTextureDesc, nullptr, texture3D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D: {}", hr);
		return false;
	}

	hr = device->CreateShaderResourceView(texture3D.Get(), &aSRVDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return false;
	}

	aTexture->mSRV = textureView;
	aTexture->mTexture3D = texture3D;

	return true;
}

Texture3D* Texture3D::Create(Array3D<uint8>& aArray3D)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Texture3D> texture3D;
	ComPtr<ID3D11ShaderResourceView> textureView;

	const uint32& arraySizeX = aArray3D.GetSizeX();
	const uint32& arraySizeY = aArray3D.GetSizeY();
	const uint32& arraySizeZ = aArray3D.GetSizeZ();

	//Setup Texture2D
	D3D11_TEXTURE3D_DESC textureDesc = {};
	textureDesc.Width = arraySizeX;
	textureDesc.Height = arraySizeY;
	textureDesc.Depth = arraySizeZ;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UINT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = aArray3D.GetData();
	initData.SysMemPitch = arraySizeX * sizeof(uint8);
	initData.SysMemSlicePitch = arraySizeY * initData.SysMemPitch;

	HRESULT hr = device->CreateTexture3D(&textureDesc, &initData, texture3D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create Texture3D: {}", hr);
		return nullptr;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture3D.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return nullptr;
	}

	Texture3D* texture = new Texture3D();
	texture->mSize = Vector3(static_cast<float>(arraySizeX), static_cast<float>(arraySizeY), static_cast<float>(arraySizeZ));
	texture->mSRV = textureView;
	texture->mTexture3D = texture3D;
	texture->mMipLevel = textureDesc.MipLevels;

	return texture;
}

Texture3D* Texture3D::CreateUAV(const Vector3& aSize)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D11Texture3D> texture3D;
	ComPtr<ID3D11ShaderResourceView> textureView;

	const uint32& arraySizeX = static_cast<uint32>(aSize.x);
	const uint32& arraySizeY = static_cast<uint32>(aSize.y);
	const uint32& arraySizeZ = static_cast<uint32>(aSize.z);

	//Setup Texture2D
	D3D11_TEXTURE3D_DESC textureDesc = {};
	textureDesc.Width = arraySizeX;
	textureDesc.Height = arraySizeY;
	textureDesc.Depth = arraySizeZ;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UINT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	HRESULT hr = device->CreateTexture3D(&textureDesc, nullptr, texture3D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create Texture3D: {}", hr);
		return nullptr;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture3D.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create SRV: {}", hr);
		return nullptr;
	}

	Texture3D* texture = new Texture3D();
	texture->mSize = Vector3(static_cast<float>(arraySizeX), static_cast<float>(arraySizeY), static_cast<float>(arraySizeZ));
	texture->mSRV = textureView;
	texture->mTexture3D = texture3D;
	texture->mMipLevel = textureDesc.MipLevels;

	return texture;
}


ID3D11ShaderResourceView* Texture3D::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** Texture3D::GetAddressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11Texture3D* Texture3D::GetTexture3D()
{
	return mTexture3D.Get();
}

ID3D11Texture3D** Texture3D::GetAdressOfTexture3D()
{
	return mTexture3D.GetAddressOf();
}

D3D11_TEXTURE3D_DESC Texture3D::GetTexture3DDesc() const
{
	D3D11_TEXTURE3D_DESC desc = {};
	mTexture3D->GetDesc(&desc);

	return desc;
}

