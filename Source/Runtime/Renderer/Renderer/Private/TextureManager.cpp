#include "TextureManager.h"

#include <d3d11.h>
#include "Renderer.h"

TextureManager::TextureManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

TextureManager::~TextureManager()
{
}

GPUResourceHandle<GPUTexture> TextureManager::CreateTexture2D(const Vector2i& Extent, RenderFormat Format, TextureBindFlags Bind, TextureUsage Usage)
{
	const LogicalDevice& device = mRenderer->GetLogicalDevice();

	GPUTexture gpuTexture{};

	DXGI_FORMAT dxFormat = ToDXFormat(Format);

	D3D11_TEXTURE2D_DESC desc{};
	desc.Height = Extent.y;
	desc.Width = Extent.x;
	desc.Format = dxFormat;
	desc.Usage = ToD11Usage(Usage);
	desc.BindFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	if (HasFlag(Bind, TextureBindFlags::RenderTarget)) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if (HasFlag(Bind, TextureBindFlags::DepthStencil)) desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	if (HasFlag(Bind, TextureBindFlags::ShaderRead)) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, gpuTexture.texture.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create Texture2D (Format={}): {}", (int)dxFormat, hr);
		return GPUResourceHandle<GPUTexture>::Invalid();
	}

	// RTV (Color)
	if (HasFlag(Bind, TextureBindFlags::RenderTarget))
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = dxFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateRenderTargetView(gpuTexture.texture.Get(), &rtvDesc, gpuTexture.rtv.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create RTV: {}", hr);
			return GPUResourceHandle<GPUTexture>::Invalid();
		}
	}

	// DSV (Depth)
	if (HasFlag(Bind, TextureBindFlags::DepthStencil))
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dxFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(gpuTexture.texture.Get(), &dsvDesc, gpuTexture.dsv.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DSV: {}", hr);
			return GPUResourceHandle<GPUTexture>::Invalid();
		}
	}

	// SRV (Shader read)
	if (HasFlag(Bind, TextureBindFlags::ShaderRead))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = dxFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(gpuTexture.texture.Get(), &srvDesc, gpuTexture.srv.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create SRV: {}", hr);
			return GPUResourceHandle<GPUTexture>::Invalid();
		}
	}

	// Viewport setup
	gpuTexture.viewport.TopLeftX = 0.0f;
	gpuTexture.viewport.TopLeftY = 0.0f;
	gpuTexture.viewport.Width = (float)Extent.x;
	gpuTexture.viewport.Height = (float)Extent.y;
	gpuTexture.viewport.MinDepth = 0.0f;
	gpuTexture.viewport.MaxDepth = 1.0f;

	gpuTexture.format = Format;
	gpuTexture.extent = Extent;

	return mTexturePool.Allocate(gpuTexture);
}

GPUResourceHandle<GPUTexture> TextureManager::CreateTextureRenderTarget(ID3D11Texture2D* Texture)
{
	const LogicalDevice& device = mRenderer->GetLogicalDevice();

	GPUTexture gpuTexture{};
	gpuTexture.texture = Texture;

	CreateTextureView(&gpuTexture);
	if (!gpuTexture.srv)
	{
		LOG_ERROR("Failed to create srv for rendertarget");
		return GPUResourceHandle<GPUTexture>::Invalid();
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	Texture->GetDesc(&textureDesc);

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = device->CreateRenderTargetView(Texture, &rtvDesc, gpuTexture.rtv.GetAddressOf());
	if (FAILED(hr))
	{
		gpuTexture.rtv.Reset();
		gpuTexture.srv.Reset();

		LOG_ERROR("Failed to create RenderTargetView: {}", hr);
		return GPUResourceHandle<GPUTexture>::Invalid();
	}

	//Setup viewport
	gpuTexture.viewport = { 0 };
	gpuTexture.viewport.TopLeftX = 0.0f;
	gpuTexture.viewport.TopLeftY = 0.0f;
	gpuTexture.viewport.Width = static_cast<float>(textureDesc.Width);
	gpuTexture.viewport.Height = static_cast<float>(textureDesc.Height);
	gpuTexture.viewport.MinDepth = 0.0f;
	gpuTexture.viewport.MaxDepth = 1.0f;

    return mTexturePool.Allocate(gpuTexture);
}

void TextureManager::CreateTextureView(GPUTexture* Texture)
{
	const LogicalDevice& device = mRenderer->GetLogicalDevice();

	D3D11_TEXTURE2D_DESC textureDesc;
	Texture->texture->GetDesc(&textureDesc);

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = device->CreateShaderResourceView(Texture->texture.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create SRV: {}", hr);
		return;
	}

	Texture->extent = Vector2i(textureDesc.Width, textureDesc.Height);
	Texture->srv = textureView;
	Texture->format = FromDXFormat(textureDesc.Format);
}
