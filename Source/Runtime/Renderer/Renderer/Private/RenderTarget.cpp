#include "pch.h"
#include "RenderTarget.h"

#include "Texture2D.h"

RenderTarget::RenderTarget()
    :
    mRTV(nullptr),
    mDSV(nullptr),
    mDSS(nullptr),
    mDepthTexture(nullptr),
    mViewport({0}), 
	mIsDepthTesting(true),
	mTexture2D(nullptr)
{
}

RenderTarget::~RenderTarget()
{
	delete mTexture2D;
	mTexture2D = nullptr;

	mRTV.Reset();
	mDSV.Reset();
	mDSS.Reset();
	mDepthTexture.Reset();
}

void RenderTarget::Resize(Vector2 aSize)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	mRTV->GetDesc(&desc);

	D3D11_TEXTURE2D_DESC depthTexture = { 0 };
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = { };
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	if (mIsDepthTesting)
	{
		mDepthTexture->GetDesc(&depthTexture);
		mDSV->GetDesc(&depthDesc);
		mDSS->GetDesc(&depthStencilDesc);
	}

	mTexture2D->Resize(aSize);

	Release();

	RenderTarget::Create(aSize, this, desc, depthTexture, depthDesc, depthStencilDesc);
}

void RenderTarget::Release()
{
	if (mRTV)
		mRTV->Release();
	if (mDSV)
		mDSV->Release();
	if (mDSS)
		mDSS->Release();
	if (mDepthTexture)
		mDepthTexture->Release();
}

void RenderTarget::Bind()
{
	Renderer::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(mDSS.Get(), 0);
	Renderer::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), mDSV.Get());
	Renderer::GetInstance()->GetDeviceContext()->RSSetViewports(1, &mViewport);
}

void RenderTarget::Clear()
{
	Vector4 color(0, 0, 0, 1);
	Renderer::GetInstance()->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), (float*)&color);
	Renderer::GetInstance()->GetDeviceContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

bool RenderTarget::IsDepthTesting()
{
	return mIsDepthTesting;
}

RenderTarget* RenderTarget::Create(const Vector2& aSize, bool EnableDepthTesting)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	RenderTarget* renderTarget = new RenderTarget();
	renderTarget->mIsDepthTesting = EnableDepthTesting;

	Texture2D* texture = Texture2D::Create(aSize);
	if (!texture)
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D");
		return nullptr;
	}

	renderTarget->mTexture2D = texture;
	const D3D11_TEXTURE2D_DESC& textureDesc = texture->GetTexture2DDesc();

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = device->CreateRenderTargetView(texture->GetTexture2D(), &rtvDesc, renderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
		return nullptr;
	}

	//Setup viewport
	renderTarget->mViewport = { 0 };
	renderTarget->mViewport.TopLeftX = 0.0f;
	renderTarget->mViewport.TopLeftY = 0.0f;
	renderTarget->mViewport.Width = static_cast<float>(textureDesc.Width);
	renderTarget->mViewport.Height = static_cast<float>(textureDesc.Height);
	renderTarget->mViewport.MinDepth = 0.0f;
	renderTarget->mViewport.MaxDepth = 1.0f;

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

		hr = device->CreateTexture2D(&depthDesc, nullptr, renderTarget->GetAdressOfDepthTexture());
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

		hr = device->CreateDepthStencilView(renderTarget->GetDepthTexture(), &dsvDesc, renderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, renderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}
	}

	return renderTarget;
}

RenderTarget* RenderTarget::Create(ID3D11Texture2D* aTexture, bool EnableDepthTesting)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	RenderTarget* renderTarget = new RenderTarget();
	renderTarget->mIsDepthTesting = EnableDepthTesting;

	Texture2D* texture = Texture2D::Create(aTexture);
	if (!texture)
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D");
		return nullptr;
	}

	renderTarget->mTexture2D = texture;
	const D3D11_TEXTURE2D_DESC& textureDesc = texture->GetTexture2DDesc();

	//Setup RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = device->CreateRenderTargetView(texture->GetTexture2D(), &rtvDesc, renderTarget->GetAdressOfRTV());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
		return nullptr;
	}

	//Setup viewport
	renderTarget->mViewport = { 0 };
	renderTarget->mViewport.TopLeftX = 0.0f;
	renderTarget->mViewport.TopLeftY = 0.0f;
	renderTarget->mViewport.Width = static_cast<float>(textureDesc.Width);
	renderTarget->mViewport.Height = static_cast<float>(textureDesc.Height);
	renderTarget->mViewport.MinDepth = 0.0f;
	renderTarget->mViewport.MaxDepth = 1.0f;

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

		hr = device->CreateTexture2D(&depthDesc, nullptr, renderTarget->GetAdressOfDepthTexture());
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

		hr = device->CreateDepthStencilView(renderTarget->GetDepthTexture(), &dsvDesc, renderTarget->GetAdressOfDSV());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, renderTarget->GetAdressOfDSS());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
			return nullptr;
		}
	}

	return renderTarget;
}

void RenderTarget::Create(
	const Vector2& aSize, 
	RenderTarget* aRenderTarget, 
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

	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	Texture2D* dx11Texture = static_cast<Texture2D*>(aRenderTarget->GetTexture2D());

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

ID3D11DepthStencilView** RenderTarget::GetAdressOfDSV()
{
    return mDSV.GetAddressOf();
}

ID3D11DepthStencilState* RenderTarget::GetDSS()
{
    return mDSS.Get();
}

ID3D11DepthStencilState** RenderTarget::GetAdressOfDSS()
{
    return mDSS.GetAddressOf();
}

ID3D11Texture2D* RenderTarget::GetDepthTexture()
{
    return mDepthTexture.Get();
}

ID3D11Texture2D** RenderTarget::GetAdressOfDepthTexture()
{
    return mDepthTexture.GetAddressOf();
}

D3D11_VIEWPORT* RenderTarget::GetViewPort()
{
    return &mViewport;
}

Texture2D* RenderTarget::GetTexture2D()
{
	return mTexture2D;
}
