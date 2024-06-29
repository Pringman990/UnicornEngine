#include "GraphicsPch.h"
#include "RenderTarget.h"

#include "DX11.h"
#include "Window.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
}

bool RenderTarget::Create(Vector2 aSize, bool EnableDepthTesting)
{
	mSize = aSize;
	mEnabledDepthTesting = EnableDepthTesting;

	ID3D11Device* device = GraphicsEngine::GetInstance().GetDX11()->GetDevice();

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

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, mTexture2D.GetAddressOf());
	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create RenderTargetView: " << errorMessage << std::endl;
		return false;
	}

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(mTexture2D.Get(), &rtvDesc, mRTV.GetAddressOf());
	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create RenderTargetView: " << errorMessage << std::endl;
		return false;
	}

	//Setup SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(mTexture2D.Get(), &srvDesc, mSRV.GetAddressOf());
	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create RenderTargetView: " << errorMessage << std::endl;
		return false;
	}

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(textureDesc.Width);
	mViewport.Height = static_cast<float>(textureDesc.Height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

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
			LPCTSTR errorMessage = err.ErrorMessage();
			std::cout << "Failed to create DepthStencil Texture2D: " << errorMessage << std::endl;
			return false;
		}

		//Setup DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, mDSV.GetAddressOf());
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errorMessage = err.ErrorMessage();
			std::cout << "Failed to create DepthStencilView: " << errorMessage << std::endl;
			return false;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = EnableDepthTesting;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, mDSS.GetAddressOf());
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errorMessage = err.ErrorMessage();
			std::cout << "Failed to create DepthStencilView: " << errorMessage << std::endl;
			return false;
		}
	}

	return true;
}

bool RenderTarget::Create(ID3D11Texture2D* aTexture2D)
{
	ID3D11Device* device = GraphicsEngine::GetInstance().GetDX11()->GetDevice();

	HRESULT hr = device->CreateRenderTargetView(
		aTexture2D,
		nullptr,
		mRTV.GetAddressOf());

	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create DepthStencilView: " << errorMessage << std::endl;
		return false;
	}

	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = (UINT)GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().resolution.x;
	desc.Height = (UINT)GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().resolution.y;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT result = device->CreateTexture2D(&desc, nullptr, &depthBufferTexture);
	if (FAILED(result))
	{
		std::cout << "Failed to create depth buffer" << std::endl;
		return false;
	}

	result = device->CreateDepthStencilView(depthBufferTexture, nullptr, mDSV.GetAddressOf());
	if (FAILED(result))
	{
		std::cout << "Failed to create Depth Stencil View" << std::endl;
		return false;
	}

	depthBufferTexture->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = device->CreateDepthStencilState(&depthStencilDesc, mDSS.GetAddressOf());
	if (FAILED(result))
	{
		std::cout << "Failed to create Depth Stencil State" << std::endl;
		return false;
	}

	return true;
}

void RenderTarget::SetAsActiveRenderTarget(ID3D11DepthStencilView* aDSV)
{
	if (aDSV)
	{
		GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), aDSV);
	}
	else
	{
		GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), nullptr);
	}
	GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext()->RSSetViewports(1, &mViewport);
}

void RenderTarget::Clear()
{
	ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext();
	Vector4 color(0, 0, 0, 0);
	context->ClearRenderTargetView(mRTV.Get(), (float*)&color);
}

void RenderTarget::Release()
{
	if (mRTV)
		mRTV->Release();
	if (mSRV)
		mSRV->Release();
	if (mDSV)
		mDSV->Release();
	if (mDSS)
		mDSS->Release();
	if (mTexture2D)
		mTexture2D->Release();
}

void RenderTarget::Resize(Vector2 aSize)
{
	Release();
	Create(aSize, mEnabledDepthTesting);
}

ID3D11ShaderResourceView* RenderTarget::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** RenderTarget::GetAdressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11RenderTargetView* RenderTarget::GetRTV()
{
	return mRTV.Get();
}

ID3D11RenderTargetView** RenderTarget::GetAdressOfRTV()
{
	return mRTV.GetAddressOf();
}

ID3D11DepthStencilView* RenderTarget::GetDSV()
{
	return mDSV.Get();
}

ID3D11DepthStencilState* RenderTarget::GetDSS()
{
	return mDSS.Get();
}

ID3D11Texture2D* RenderTarget::GetTexture2D()
{
	return mTexture2D.Get();
}
