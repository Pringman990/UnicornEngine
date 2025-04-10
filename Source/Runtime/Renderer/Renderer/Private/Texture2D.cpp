#include "pch.h"
#include "Texture2D.h"

#include <DDSTextureLoader.h>
#include <ResourceUploadBatch.h>

Texture2D::Texture2D()
	:
	mSRV(nullptr),
	mSRVHandle({}),
	mSize(Vector2(0,0)),
	mMipLevel(0)
{
}

Texture2D::~Texture2D()
{
	Release();
	//mSRV.Reset();
}

void Texture2D::Resize(const Vector2& aNewSize)
{
	D3D12_RESOURCE_DESC oldSrvDesc = mSRV->GetDesc();
	Release();
	if (!Create(this, aNewSize, oldSrvDesc))
	{
		_LOG_RENDERER_ERROR("Failed to resize Texture2D");
	}
}

void Texture2D::Release()
{
	if (mSRV)
	{
		Renderer::GetInstance()->GetSRVHeapManager().Free(mSRVHandle);
		mSRV.Reset();
	}
}

Texture2D* Texture2D::Create(const std::wstring& aDDSPath)
{
	Renderer* renderer = Renderer::GetInstance();
	Texture2D* texture = new Texture2D();

	DirectX::ResourceUploadBatch uploadBatch(renderer->GetDevice());
	uploadBatch.Begin();

	bool isCubeMap = false;
	HRESULT result = DirectX::CreateDDSTextureFromFile(
		renderer->GetDevice(),
		uploadBatch,
		aDDSPath.c_str(),
		texture->mSRV.GetAddressOf(),
		false,
		0ui64,
		nullptr,
		&isCubeMap
		);

	if (isCubeMap)
	{
		_LOG_RENDERER_ERROR("Tried to load cubemap dds texture on a texture2d, path: {}", WStringToString(aDDSPath));
		delete texture;
		return nullptr;
	}

	if (FAILED(result))
	{
		delete texture;
		_LOG_RENDERER_ERROR("Failed to load dds texture, path: {}, hr: {}", WStringToString(aDDSPath), result);
		return nullptr;
	}

	std::future<void> uploadFinish = uploadBatch.End(renderer->GetCommandQueue());
	uploadFinish.wait();

	D3D12_RESOURCE_DESC textureDesc = texture->mSRV->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	texture->mSRVHandle = renderer->GetSRVHeapManager().Allocate();
	renderer->GetDevice()->CreateShaderResourceView(texture->mSRV.Get(), &srvDesc, texture->mSRVHandle);

	texture->mSize = Vector2(textureDesc.Width, textureDesc.Height);
	texture->mMipLevel = textureDesc.MipLevels;

	return texture;
}

Texture2D* Texture2D::Create(const Vector2& aSize, D3D12_RESOURCE_FLAGS someFlags)
{
	if (aSize.x <= 0 || aSize.y <= 0)
	{
		_LOG_RENDERER_ERROR("Failed to create texture2d, size invalid");
		return nullptr;
	}

	Renderer* renderer = Renderer::GetInstance();
	ID3D12Device* device = renderer->GetDevice();

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Alignment = 0;
	textureDesc.Width = static_cast<uint32>(aSize.x);
	textureDesc.Height = static_cast<uint32>(aSize.y);
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Flags = someFlags;

	Texture2D* texture = new Texture2D();

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = 0.f;
	clearValue.Color[1] = 0.f;
	clearValue.Color[2] = 0.f;
	clearValue.Color[3] = 0.f;

	D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		&clearValue,
		IID_PPV_ARGS(texture->mSRV.GetAddressOf())
	);
	if (FAILED(hr))
	{
		delete texture;
		_LOG_RENDERER_ERROR("Failed to create texture2d resource. {}", hr);
		return nullptr;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	texture->mSRVHandle = renderer->GetSRVHeapManager().Allocate();
	device->CreateShaderResourceView(texture->mSRV.Get(), &srvDesc, texture->mSRVHandle);

	texture->mSize = Vector2(textureDesc.Width, textureDesc.Height);
	texture->mMipLevel = textureDesc.MipLevels;

	return texture;
}

bool Texture2D::Create(Texture2D* aTexture, const Vector2& aNewSize, D3D12_RESOURCE_DESC aTextureDesc)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D12Device* device = renderer->GetDevice();

	aTextureDesc.Width = static_cast<uint32>(aNewSize.x);
	aTextureDesc.Height = static_cast<uint32>(aNewSize.y);

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = 0.f;
	clearValue.Color[1] = 0.f;
	clearValue.Color[2] = 0.f;
	clearValue.Color[3] = 0.f;

	D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&aTextureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		&clearValue,
		IID_PPV_ARGS(aTexture->mSRV.GetAddressOf())
	);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create texture2d resource. {}", hr);
		return false;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = aTextureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = aTextureDesc.MipLevels;

	aTexture->mSRVHandle = renderer->GetSRVHeapManager().Allocate();
	device->CreateShaderResourceView(aTexture->mSRV.Get(), &srvDesc, aTexture->mSRVHandle);

	aTexture->mSize = aNewSize;
	aTexture->mMipLevel = aTextureDesc.MipLevels;

	return true;
}

Texture2D* Texture2D::Create(ID3D12Resource* aRTVResource)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D12Device* device = renderer->GetDevice();

	if (!aRTVResource)
	{
		_LOG_RENDERER_ERROR("Invalid RTV resource provided to Texture2D::Create");
		return nullptr;
	}

	D3D12_RESOURCE_DESC resourceDesc = aRTVResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resourceDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;

	Texture2D* texture = new Texture2D();
	texture->mSRV = aRTVResource;
	texture->mSize = Vector2(static_cast<float>(resourceDesc.Width), static_cast<float>(resourceDesc.Height));
	texture->mMipLevel = resourceDesc.MipLevels;
	texture->mSRVHandle = renderer->GetSRVHeapManager().Allocate();
	device->CreateShaderResourceView(aRTVResource, &srvDesc, texture->mSRVHandle);

	return texture;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture2D::GetSRVHandle() const
{
	return mSRVHandle;
}

ComPtr<ID3D12Resource>& Texture2D::GetResource()
{
	return mSRV;
}

D3D12_RESOURCE_DESC Texture2D::GetDesc() const
{
	return mSRV->GetDesc();
}
