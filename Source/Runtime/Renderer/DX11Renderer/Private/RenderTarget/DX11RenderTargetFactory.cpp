#include "pch.h"
#include "RenderTarget/DX11RenderTargetFactory.h"

#include "DX11RenderTarget.h"
#include "DX11RenderingBackend.h"
#include "Texture/DX11Texture.h"
#include "Texture/DX11TextureFactory.h"
#include "Application.h"

DX11RenderTargetFactory::DX11RenderTargetFactory()
{
}

DX11RenderTargetFactory::~DX11RenderTargetFactory()
{
}

RenderTarget* DX11RenderTargetFactory::CreateRenderTarget(const Vector2& aSize, bool EnableDepthTesting)
{
	return _CreateRenderTarget(aSize, EnableDepthTesting);
}

DX11RenderTarget* DX11RenderTargetFactory::_CreateRenderTarget(const Vector2& aSize, bool EnableDepthTesting)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

	DX11RenderTarget* dx11RenderTarget = new DX11RenderTarget();
	dx11RenderTarget->mIsDepthTesting = EnableDepthTesting;

	DX11Texture* dx11Texture = DX11TextureFactory::CreateTexture(aSize);
	if (!dx11Texture)
	{
		_LOG_RENDERER_ERROR("Texture was not a DX11 texture when using DX11?");
		return nullptr;
	}

	dx11RenderTarget->mTexture = dx11Texture;
	const D3D11_TEXTURE2D_DESC& textureDesc = dx11Texture->GetTexture2DDesc();

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = device->CreateRenderTargetView(dx11Texture->GetTexture2D(), &rtvDesc, dx11RenderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
		return nullptr;
	}

	//Setup viewport
	dx11RenderTarget->mViewport = { 0 };
	dx11RenderTarget->mViewport.TopLeftX = 0.0f;
	dx11RenderTarget->mViewport.TopLeftY = 0.0f;
	dx11RenderTarget->mViewport.Width = static_cast<float>(textureDesc.Width);
	dx11RenderTarget->mViewport.Height = static_cast<float>(textureDesc.Height);
	dx11RenderTarget->mViewport.MinDepth = 0.0f;
	dx11RenderTarget->mViewport.MaxDepth = 1.0f;

	if (EnableDepthTesting)
	{
		// Setup depth buffer texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = textureDesc.Width;
		depthDesc.Height = textureDesc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = device->CreateTexture2D(&depthDesc, nullptr, dx11RenderTarget->GetAdressOfDepthTexture());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", hr);
			return nullptr;
		}

		//Setup DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(dx11RenderTarget->GetDepthTexture(), &dsvDesc, dx11RenderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, dx11RenderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}
	}

	return dx11RenderTarget;
}

void DX11RenderTargetFactory::_CreateRenderTarget(const Vector2& aSize,
	DX11RenderTarget* aRenderTarget,
	D3D11_RENDER_TARGET_VIEW_DESC aRTVDesc,
	D3D11_TEXTURE2D_DESC aDepthTexture,
	D3D11_DEPTH_STENCIL_VIEW_DESC aDepthDesc,
	D3D11_DEPTH_STENCIL_DESC aDepthStencilDesc
)
{
	if (aRenderTarget == nullptr)
	{
		_LOG_RENDERER_ERROR("RenderTarget was nullptr, please pass a valid rendertarget");
		return;
	}

	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();
	DX11Texture* dx11Texture = static_cast<DX11Texture*>(aRenderTarget->GetTexture());

	HRESULT hr = device->CreateRenderTargetView(dx11Texture->GetTexture2D(), &aRTVDesc, aRenderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
		return;
	}

	D3D11_VIEWPORT* viewPort = aRenderTarget->GetViewPort();
	viewPort->Width = aSize.x;
	viewPort->Height = aSize.y;

	if (aRenderTarget->IsDepthTesting())
	{
		aDepthTexture.Width = static_cast<uint32>(aSize.x);
		aDepthTexture.Height = static_cast<uint32>(aSize.y);

		hr = device->CreateTexture2D(&aDepthTexture, nullptr, aRenderTarget->mDepthTexture.GetAddressOf());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", hr);
			return;
		}
		
		hr = device->CreateDepthStencilView(aRenderTarget->mDepthTexture.Get(), &aDepthDesc, aRenderTarget->mDSV.GetAddressOf());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return;
		}

		hr = device->CreateDepthStencilState(&aDepthStencilDesc, aRenderTarget->mDSS.GetAddressOf());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilState: {}", hr);
			return;
		}
	}
}

DX11RenderTarget* DX11RenderTargetFactory::_CreateRenderTarget(ID3D11Texture2D* aTexture, bool EnableDepthTesting)
{
	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();

	DX11RenderTarget* dx11RenderTarget = new DX11RenderTarget();
	dx11RenderTarget->mIsDepthTesting = EnableDepthTesting;

	DX11Texture* dx11Texture = DX11TextureFactory::CreateTexture(aTexture);
	if (!dx11Texture)
	{
		_LOG_RENDERER_ERROR("Texture was not a DX11 texture when using DX11?");
		return nullptr;
	}

	dx11RenderTarget->mTexture = dx11Texture;
	const D3D11_TEXTURE2D_DESC& textureDesc = dx11Texture->GetTexture2DDesc();

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = device->CreateRenderTargetView(dx11Texture->GetTexture2D(), &rtvDesc, dx11RenderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
		return nullptr;
	}

	//Setup viewport
	dx11RenderTarget->mViewport = { 0 };
	dx11RenderTarget->mViewport.TopLeftX = 0.0f;
	dx11RenderTarget->mViewport.TopLeftY = 0.0f;
	dx11RenderTarget->mViewport.Width = static_cast<float>(textureDesc.Width);
	dx11RenderTarget->mViewport.Height = static_cast<float>(textureDesc.Height);
	dx11RenderTarget->mViewport.MinDepth = 0.0f;
	dx11RenderTarget->mViewport.MaxDepth = 1.0f;

	if (EnableDepthTesting)
	{
		// Setup depth buffer texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = textureDesc.Width;
		depthDesc.Height = textureDesc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = device->CreateTexture2D(&depthDesc, nullptr, dx11RenderTarget->GetAdressOfDepthTexture());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", hr);
			return nullptr;
		}

		//Setup DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(dx11RenderTarget->GetDepthTexture(), &dsvDesc, dx11RenderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, dx11RenderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}
	}

	return dx11RenderTarget;
}
