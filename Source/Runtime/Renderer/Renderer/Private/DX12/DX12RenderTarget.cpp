//#include "pch.h"
//#include "RenderTarget.h"
//
//#include "Texture2D.h"
//
//RenderTarget::RenderTarget()
//    :
//	mRTVHandle({}),
//	mDSVHandle({}),
//	mDDSDesc({}),
//	mRTV(nullptr),
//	mDepthStencilBuffer(nullptr),
//	mViewport({}),
//	mScissorRect({}),
//	mIsDepthTesting(true),
//	mSRV(nullptr)
//{
//}
//
//RenderTarget::~RenderTarget()
//{
//	Release(false);
//
//	delete mSRV;
//	mSRV = nullptr;
//}
//
//void RenderTarget::Resize(Vector2 Size)
//{
//	D3D12_RESOURCE_DESC rtvDesc = mRTV->GetDesc();
//	D3D12_RESOURCE_DESC dsvDesc = {};
//	if (mIsDepthTesting)
//	{
//		dsvDesc = mDepthStencilBuffer->GetDesc();
//	}
//
//	Release();
//	mSRV->Resize(Size);
//
//	RenderTarget::Create(Size, this, rtvDesc, dsvDesc);
//}
//
//void RenderTarget::Release(bool ReleaseSRV)
//{
//	if (mRTV)
//	{
//		Renderer::Get()->GetRTVHeapManager().Free(mRTVHandle);
//		mRTV.Reset();
//		mRTVHandle = {};
//	}
//	
//	if (mDepthStencilBuffer) 
//	{
//		Renderer::Get()->GetDSVHeapManager().Free(mDSVHandle);
//		mDepthStencilBuffer.Reset();
//		
//		mDSVHandle = {};
//		mDDSDesc = {};
//	}
//
//	if (mSRV && ReleaseSRV)
//		mSRV->Release();
//}
//
//bool RenderTarget::IsDepthTesting()
//{
//	return mIsDepthTesting;
//}
//
//RenderTarget* RenderTarget::Create(const Vector2& Size, bool EnableDepthTesting)
//{
//	Texture2D* texture = Texture2D::Create(Size, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
//	if (!texture)
//	{
//		_LOG_RENDERER_ERROR("Failed to create Texture2D");
//		return nullptr;
//	}
//
//	RenderTarget* renderTarget = new RenderTarget();
//	renderTarget->mIsDepthTesting = EnableDepthTesting;
//	renderTarget->mSRV = texture;
//	renderTarget->mRTV = texture->GetResource();
//
//	if (!renderTarget->CreateInternal(texture->GetResource().Get(), EnableDepthTesting))
//	{
//		_LOG_RENDERER_ERROR("Failed to create RenderTarget");
//		return nullptr;
//	}
//
//	return renderTarget;
//}
//
//RenderTarget* RenderTarget::Create(ID3D12Resource* RTVResource, bool EnableDepthTesting)
//{
//	Texture2D* texture = Texture2D::Create(RTVResource);
//	if (!texture)
//	{
//		_LOG_RENDERER_ERROR("Failed to create Texture2D");
//		return nullptr;
//	}
//
//	RenderTarget* renderTarget = new RenderTarget();
//	renderTarget->mIsDepthTesting = EnableDepthTesting;
//	renderTarget->mSRV = texture;
//	renderTarget->mRTV = RTVResource;
//
//	if (!renderTarget->CreateInternal(texture->GetResource().Get(), EnableDepthTesting))
//	{
//		_LOG_RENDERER_ERROR("Failed to create RenderTarget");
//		return nullptr;
//	}
//
//	return renderTarget;
//}
//
//ComPtr<ID3D12Resource>& RenderTarget::GetResource()
//{
//	return mRTV;
//}
//
//const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTarget::GetRTVHandle()
//{
//	return mRTVHandle;
//}
//
//const D3D12_GPU_DESCRIPTOR_HANDLE& RenderTarget::GetGPUHandle()
//{
//	return mGPUHandle;
//}
//
//const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTarget::GetDSVHandle()
//{
//	return mDSVHandle;
//}
//
//D3D12_VIEWPORT& RenderTarget::GetViewport()
//{
//	return mViewport;
//}
//
//D3D12_RECT& RenderTarget::GetScissorRect()
//{
//	return mScissorRect;
//}
//
//Texture2D* RenderTarget::GetTexture()
//{
//	return mSRV;
//}
//
//bool RenderTarget::CreateInternal(ID3D12Resource* RTVResource, bool EnableDepthTesting)
//{
//	Renderer* renderer = Renderer::Get();
//	ID3D12Device* device = renderer->GetDevice();
//
//	D3D12_RESOURCE_DESC textureDesc = RTVResource->GetDesc();
//	
//	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
//	rtvDesc.Format = textureDesc.Format;
//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
//	rtvDesc.Texture2D.MipSlice = 0;
//
//	mRTVHandle = renderer->GetRTVHeapManager().Allocate();
//	mGPUHandle = Renderer::Get()->GetRTVHeapManager().GetGPUHandleFromCPUHandle(mRTVHandle);
//	device->CreateRenderTargetView(RTVResource, &rtvDesc, mRTVHandle);
//
//	//Setup viewport
//	mViewport = { 0 };
//	mViewport.TopLeftX = 0.0f;
//	mViewport.TopLeftY = 0.0f;
//	mViewport.Width = static_cast<float>(textureDesc.Width);
//	mViewport.Height = static_cast<float>(textureDesc.Height);
//	mViewport.MinDepth = 0.0f;
//	mViewport.MaxDepth = 1.0f;
//
//	mScissorRect.left = 0;
//	mScissorRect.top = 0;
//	mScissorRect.right = textureDesc.Width;
//	mScissorRect.bottom = textureDesc.Height;
//
//	if (EnableDepthTesting)
//	{
//		DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;
//
//		D3D12_RESOURCE_DESC depthDesc = {};
//		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//		depthDesc.Alignment = 0;
//		depthDesc.Width = textureDesc.Width;
//		depthDesc.Height = textureDesc.Height;
//		depthDesc.DepthOrArraySize = 1;
//		depthDesc.MipLevels = 1;
//		depthDesc.Format = depthFormat;
//		depthDesc.SampleDesc.Count = 1; // No MSAA for depth buffer
//		depthDesc.SampleDesc.Quality = 0;
//		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//	
//		D3D12_CLEAR_VALUE clearValue = {};
//		clearValue.Format = depthFormat;
//		clearValue.DepthStencil.Depth = 1.0f;
//		clearValue.DepthStencil.Stencil = 0;
//		
//		D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
//		HRESULT hr = device->CreateCommittedResource(
//			&heapProperties,
//			D3D12_HEAP_FLAG_NONE,
//			&depthDesc,
//			D3D12_RESOURCE_STATE_DEPTH_WRITE,
//			&clearValue,
//			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
//		);
//		if (FAILED(hr))
//		{
//			_LOG_RENDERER_ERROR("Failed to create DepthStencil resource: {}", hr);
//			return false;
//		}
//
//		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//		dsvDesc.Format = depthFormat;
//		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//		dsvDesc.Texture2D.MipSlice = 0;
//
//		mDSVHandle = renderer->GetDSVHeapManager().Allocate();
//		device->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mDSVHandle);
//
//		mDDSDesc = {};
//		mDDSDesc.DepthEnable = TRUE;
//		mDDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//		mDDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//		mDDSDesc.StencilEnable = FALSE;
//	}
//	return true;
//}
//
//void RenderTarget::Create(
//	const Vector2& Size,
//	RenderTarget* RenderTarget,
//	D3D12_RESOURCE_DESC RTVDesc,
//	D3D12_RESOURCE_DESC DepthDesc
//)
//{
//	if (RenderTarget == nullptr)
//	{
//		_LOG_RENDERER_ERROR("RenderTarget was nullptr, please pass a valid rendertarget");
//		return;
//	}
//
//	Renderer* renderer = Renderer::Get();
//	ID3D12Device* device = renderer->GetDevice();
//
//	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
//	rtvDesc.Format = RTVDesc.Format;
//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
//	rtvDesc.Texture2D.MipSlice = 0;
//
//	RenderTarget->mRTVHandle = renderer->GetRTVHeapManager().Allocate();
//	RenderTarget->mGPUHandle = Renderer::Get()->GetRTVHeapManager().GetGPUHandleFromCPUHandle(RenderTarget->mRTVHandle);
//	device->CreateRenderTargetView(RenderTarget->mSRV->GetResource().Get(), &rtvDesc, RenderTarget->mRTVHandle);
//
//	//Setup viewport
//	RenderTarget->mViewport = { 0 };
//	RenderTarget->mViewport.TopLeftX = 0.0f;
//	RenderTarget->mViewport.TopLeftY = 0.0f;
//	RenderTarget->mViewport.Width = Size.x;
//	RenderTarget->mViewport.Height = Size.y;
//	RenderTarget->mViewport.MinDepth = 0.0f;
//	RenderTarget->mViewport.MaxDepth = 1.0f;
//
//	RenderTarget->mScissorRect.left = 0;
//	RenderTarget->mScissorRect.top = 0;
//	RenderTarget->mScissorRect.right = static_cast<uint32>(Size.x);
//	RenderTarget->mScissorRect.bottom = static_cast<uint32>(Size.y);
//
//	if (RenderTarget->mIsDepthTesting)
//	{
//		DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT;
//		DepthDesc.Width = static_cast<uint32>(Size.x);
//		DepthDesc.Height = static_cast<uint32>(Size.y);
//
//		D3D12_CLEAR_VALUE clearValue = {};
//		clearValue.Format = depthFormat;
//		clearValue.DepthStencil.Depth = 1.0f;
//		clearValue.DepthStencil.Stencil = 0;
//
//		D3D12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
//		HRESULT hr = device->CreateCommittedResource(
//			&heapProperties,
//			D3D12_HEAP_FLAG_NONE,
//			&DepthDesc,
//			D3D12_RESOURCE_STATE_DEPTH_WRITE,
//			&clearValue,
//			IID_PPV_ARGS(RenderTarget->mDepthStencilBuffer.GetAddressOf())
//		);
//		if (FAILED(hr))
//		{
//			_LOG_RENDERER_ERROR("Failed to create DepthStencil resource: {}", hr);
//			return;
//		}
//
//		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//		dsvDesc.Format = depthFormat;
//		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//		dsvDesc.Texture2D.MipSlice = 0;
//
//		RenderTarget->mDSVHandle = renderer->GetDSVHeapManager().Allocate();
//		device->CreateDepthStencilView(RenderTarget->mDepthStencilBuffer.Get(), &dsvDesc, RenderTarget->mDSVHandle);
//	}
//}
