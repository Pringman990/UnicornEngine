#include "pch.h"
#include "RenderTarget.h"

#include "Texture2D.h"

RenderTarget::RenderTarget()
    :
	mRTVHandle({}),
	mDSVHandle({}),
	mDDSDesc({}),
	mRTV(nullptr),
	mDepthStencilBuffer(nullptr),
	mViewport({}),
	mScissorRect({}),
	mIsDepthTesting(true),
	mSRV(nullptr)
{
}

RenderTarget::~RenderTarget()
{
	Release(false);

	delete mSRV;
	mSRV = nullptr;
}

void RenderTarget::Resize(Vector2 aSize)
{
	D3D12_RESOURCE_DESC rtvDesc = mRTV->GetDesc();
	D3D12_RESOURCE_DESC dsvDesc = {};
	if (mIsDepthTesting)
	{
		dsvDesc = mDepthStencilBuffer->GetDesc();
	}

	Release();
	mSRV->Resize(aSize);

	RenderTarget::Create(aSize, this, rtvDesc, dsvDesc);
}

void RenderTarget::Release(bool ReleaseSRV)
{
	if (mRTV)
	{
		Renderer::GetInstance()->GetRTVHeapManager().Free(mRTVHandle);
		mRTV.Reset();
		mRTVHandle = {};
	}
	
	if (mDepthStencilBuffer) 
	{
		Renderer::GetInstance()->GetDSVHeapManager().Free(mDSVHandle);
		mDepthStencilBuffer.Reset();
		
		mDSVHandle = {};
		mDDSDesc = {};
	}

	if (mSRV && ReleaseSRV)
		mSRV->Release();
}

bool RenderTarget::IsDepthTesting()
{
	return mIsDepthTesting;
}

RenderTarget* RenderTarget::Create(const Vector2& aSize, bool EnableDepthTesting)
{
	Texture2D* texture = Texture2D::Create(aSize, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	if (!texture)
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D");
		return nullptr;
	}

	RenderTarget* renderTarget = new RenderTarget();
	renderTarget->mIsDepthTesting = EnableDepthTesting;
	renderTarget->mSRV = texture;
	renderTarget->mRTV = texture->GetResource();

	if (!renderTarget->CreateInternal(texture->GetResource().Get(), EnableDepthTesting))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTarget");
		return nullptr;
	}

	return renderTarget;
}

RenderTarget* RenderTarget::Create(ID3D12Resource* aRTVResource, bool EnableDepthTesting)
{
	Texture2D* texture = Texture2D::Create(aRTVResource);
	if (!texture)
	{
		_LOG_RENDERER_ERROR("Failed to create Texture2D");
		return nullptr;
	}

	RenderTarget* renderTarget = new RenderTarget();
	renderTarget->mIsDepthTesting = EnableDepthTesting;
	renderTarget->mSRV = texture;
	renderTarget->mRTV = aRTVResource;

	if (!renderTarget->CreateInternal(texture->GetResource().Get(), EnableDepthTesting))
	{
		_LOG_RENDERER_ERROR("Failed to create RenderTarget");
		return nullptr;
	}

	return renderTarget;
}

ComPtr<ID3D12Resource>& RenderTarget::GetResource()
{
	return mRTV;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTarget::GetRTVHandle()
{
	return mRTVHandle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& RenderTarget::GetGPUHandle()
{
	return mGPUHandle;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTarget::GetDSVHandle()
{
	return mDSVHandle;
}

D3D12_VIEWPORT& RenderTarget::GetViewport()
{
	return mViewport;
}

D3D12_RECT& RenderTarget::GetScissorRect()
{
	return mScissorRect;
}

Texture2D* RenderTarget::GetTexture()
{
	return mSRV;
}

bool RenderTarget::CreateInternal(ID3D12Resource* aRTVResource, bool EnableDepthTesting)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D12Device* device = renderer->GetDevice();

	D3D12_RESOURCE_DESC textureDesc = aRTVResource->GetDesc();
	
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	mRTVHandle = renderer->GetRTVHeapManager().Allocate();
	mGPUHandle = Renderer::GetInstance()->GetRTVHeapManager().GetGPUHandleFromCPUHandle(mRTVHandle);
	device->CreateRenderTargetView(aRTVResource, &rtvDesc, mRTVHandle);

	//Setup viewport
	mViewport = { 0 };
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(textureDesc.Width);
	mViewport.Height = static_cast<float>(textureDesc.Height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissorRect.left = 0;
	mScissorRect.top = 0;
	mScissorRect.right = textureDesc.Width;
	mScissorRect.bottom = textureDesc.Height;

	if (EnableDepthTesting)
	{
		DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;

		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthDesc.Alignment = 0;
		depthDesc.Width = textureDesc.Width;
		depthDesc.Height = textureDesc.Height;
		depthDesc.DepthOrArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Format = depthFormat;
		depthDesc.SampleDesc.Count = 1; // No MSAA for depth buffer
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = depthFormat;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		
		D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
		);
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencil resource: {}", hr);
			return false;
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		mDSVHandle = renderer->GetDSVHeapManager().Allocate();
		device->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mDSVHandle);

		mDDSDesc = {};
		mDDSDesc.DepthEnable = TRUE;
		mDDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		mDDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		mDDSDesc.StencilEnable = FALSE;
	}
	return true;
}

void RenderTarget::Create(
	const Vector2& aSize,
	RenderTarget* aRenderTarget,
	D3D12_RESOURCE_DESC aRTVDesc,
	D3D12_RESOURCE_DESC aDepthDesc
)
{
	if (aRenderTarget == nullptr)
	{
		_LOG_RENDERER_ERROR("RenderTarget was nullptr, please pass a valid rendertarget");
		return;
	}

	Renderer* renderer = Renderer::GetInstance();
	ID3D12Device* device = renderer->GetDevice();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = aRTVDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	aRenderTarget->mRTVHandle = renderer->GetRTVHeapManager().Allocate();
	aRenderTarget->mGPUHandle = Renderer::GetInstance()->GetRTVHeapManager().GetGPUHandleFromCPUHandle(aRenderTarget->mRTVHandle);
	device->CreateRenderTargetView(aRenderTarget->mSRV->GetResource().Get(), &rtvDesc, aRenderTarget->mRTVHandle);

	//Setup viewport
	aRenderTarget->mViewport = { 0 };
	aRenderTarget->mViewport.TopLeftX = 0.0f;
	aRenderTarget->mViewport.TopLeftY = 0.0f;
	aRenderTarget->mViewport.Width = aSize.x;
	aRenderTarget->mViewport.Height = aSize.y;
	aRenderTarget->mViewport.MinDepth = 0.0f;
	aRenderTarget->mViewport.MaxDepth = 1.0f;

	aRenderTarget->mScissorRect.left = 0;
	aRenderTarget->mScissorRect.top = 0;
	aRenderTarget->mScissorRect.right = static_cast<uint32>(aSize.x);
	aRenderTarget->mScissorRect.bottom = static_cast<uint32>(aSize.y);

	if (aRenderTarget->mIsDepthTesting)
	{
		DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;
		aDepthDesc.Width = static_cast<uint32>(aSize.x);
		aDepthDesc.Height = static_cast<uint32>(aSize.y);

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = depthFormat;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&aDepthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(aRenderTarget->mDepthStencilBuffer.GetAddressOf())
		);
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create DepthStencil resource: {}", hr);
			return;
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		aRenderTarget->mDSVHandle = renderer->GetDSVHeapManager().Allocate();
		device->CreateDepthStencilView(aRenderTarget->mDepthStencilBuffer.Get(), &dsvDesc, aRenderTarget->mDSVHandle);
	}
}
