#include "pch.h"
#include "DX11RenderTargetFactory.h"

#include "DX11RenderTargetBackend.h"
#include "DX11Renderer.h"
#include "Application.h"

DX11RenderTargetFactory::DX11RenderTargetFactory()
{
}

DX11RenderTargetFactory::~DX11RenderTargetFactory()
{
}

IRenderTargetBackend* DX11RenderTargetFactory::CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting)
{
	return _CreateRenderTarget(aSize, EnableDepthTesting);
}

DX11RenderTargetBackend* DX11RenderTargetFactory::_CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting)
{
	DX11RenderTargetBackend* dx11RenderTarget = new DX11RenderTargetBackend();
	dx11RenderTarget->SetSize(aSize);
	dx11RenderTarget->SetEnabledDepthTesting(EnableDepthTesting);

	ID3D11Device* device = DX11Renderer::_GetInstance()->GetDevice();

	//Setup Texture2D
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<uint32_t>(aSize.x);
	textureDesc.Height = static_cast<uint32_t>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, dx11RenderTarget->GetAddressOfTexture2D());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return nullptr;
	}

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(dx11RenderTarget->GetTexture2D(), &rtvDesc, dx11RenderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return nullptr;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(dx11RenderTarget->GetTexture2D(), &srvDesc, dx11RenderTarget->GetAdressOfSRV());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return nullptr;
	}

	D3D11_VIEWPORT* viewPort = dx11RenderTarget->GetViewPort();
	viewPort->TopLeftX = 0.0f;
	viewPort->TopLeftY = 0.0f;
	viewPort->Width = static_cast<float>(textureDesc.Width);
	viewPort->Height = static_cast<float>(textureDesc.Height);
	viewPort->MinDepth = 0.0f;
	viewPort->MaxDepth = 1.0f;

	if (EnableDepthTesting)
	{
		// Setup depth buffer texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = static_cast<UINT>(aSize.x);
		depthDesc.Height = static_cast<UINT>(aSize.y);
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ComPtr<ID3D11Texture2D> depthStencilTexture;
		hr = device->CreateTexture2D(&depthDesc, nullptr, depthStencilTexture.GetAddressOf());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", msg);
			return nullptr;
		}

		//Setup DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, dx11RenderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", msg);
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = EnableDepthTesting;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, dx11RenderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", msg);
			return nullptr;
		}
	}

	return dx11RenderTarget;
}

void DX11RenderTargetFactory::_CreateRenderTarget(Vector2 aSize, DX11RenderTargetBackend* aRenderTarget)
{
	if (aRenderTarget == nullptr)
	{
		LOG_RENDERER_ERROR("RenderTarget was nullptr, please pass a valid rendertarget");
		return;
	}

	aRenderTarget->SetSize(aSize);

	ID3D11Device* device = DX11Renderer::_GetInstance()->GetDevice();

	//Setup Texture2D
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<uint32_t>(aSize.x);
	textureDesc.Height = static_cast<uint32_t>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, aRenderTarget->GetAddressOfTexture2D());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return;
	}

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(aRenderTarget->GetTexture2D(), &rtvDesc, aRenderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(aRenderTarget->GetTexture2D(), &srvDesc, aRenderTarget->GetAdressOfSRV());
	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", msg);
		return;
	}

	D3D11_VIEWPORT* viewPort = aRenderTarget->GetViewPort();
	viewPort->TopLeftX = 0.0f;
	viewPort->TopLeftY = 0.0f;
	viewPort->Width = static_cast<float>(textureDesc.Width);
	viewPort->Height = static_cast<float>(textureDesc.Height);
	viewPort->MinDepth = 0.0f;
	viewPort->MaxDepth = 1.0f;

	if (aRenderTarget->IsDepthTesting())
	{
		// Setup depth buffer texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = static_cast<UINT>(aSize.x);
		depthDesc.Height = static_cast<UINT>(aSize.y);
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ComPtr<ID3D11Texture2D> depthStencilTexture;
		hr = device->CreateTexture2D(&depthDesc, nullptr, depthStencilTexture.GetAddressOf());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", msg);
			return;
		}

		//Setup DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, aRenderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", msg);
			return;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = aRenderTarget->IsDepthTesting();
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, aRenderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_com_error err(hr);
			FString msg = err.ErrorMessage();
			LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", msg);
			return;
		}
	}
}

DX11RenderTargetBackend* DX11RenderTargetFactory::_CreateRenderTarget(ID3D11Texture2D* aTexture2D)
{
	DX11RenderTargetBackend* dx11RenderTarget = new DX11RenderTargetBackend();

	ID3D11Device* device = DX11Renderer::_GetInstance()->GetDevice();

	HRESULT hr = device->CreateRenderTargetView(
		aTexture2D,
		nullptr,
		dx11RenderTarget->GetAdressOfRTV());

	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", msg);
		return nullptr;
	}

	const IWindowInfo& windowInfo = Application::GetApp()->GetWindowInfo();

	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = windowInfo.viewportWidth;
	desc.Height = windowInfo.windowHeight;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT result = device->CreateTexture2D(&desc, nullptr, &depthBufferTexture);
	if (FAILED(result))
	{
		LOG_RENDERER_ERROR("Failed to create depth buffer");
		return nullptr;
	}

	result = device->CreateDepthStencilView(depthBufferTexture, nullptr, dx11RenderTarget->GetAdressOfDSV());
	if (FAILED(result))
	{
		LOG_RENDERER_ERROR("Failed to create Depth Stencil View");
		return nullptr;
	}

	depthBufferTexture->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = device->CreateDepthStencilState(&depthStencilDesc, dx11RenderTarget->GetAdressOfDSS());
	if (FAILED(result))
	{
		LOG_RENDERER_ERROR("Failed to create Depth Stencil State");
		return nullptr;
	}

	return dx11RenderTarget;
}
