//#include "pch.h"
//#include "Renderer.h"
//
//#include "RenderTarget.h"
//#include "ConstantBuffer/ConstantBuffer.h"
//
//#include <Application/Windows/WindowsApplication.h>
//
//#include "Vertex.h"
//#include "Camera.h"
//#include "Texture2D.h"
//
//#include <dxgi1_5.h>
//#include <dxgidebug.h>
//
//Renderer::Renderer()
//	:
//	mDevice(nullptr),
//	mCommandQueue(nullptr),
//	mSwapChain(nullptr),
//	mDXGIFactory(nullptr),
//	mMainCommandList(nullptr),
//	mFence(nullptr),
//	mFenceEvent(nullptr),
//	mFenceValue(0),
//	mFrameIndex(0),
//	mRTVHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64),
//	mDSVHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 64),
//	mSRVHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SRV_HEAP_SIZE),
//	mSamplerHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 16),
//	mClearColor(Color(0.5f, 0.5f, 0.7f, 0)),
//	mVsync(true),
//	mDrawCalls(0),
//	mIsResizing(false)
//{
//
//}
//
//Renderer::~Renderer()
//{
//	WaitForGPU();
//
//	for (uint32 i = 0; i < BACKBUFFER_COUNT; i++)
//	{
//		delete mBackBuffers[i];
//		mBackBuffers[i] = nullptr;
//		mMainCommandAllocators[i].Reset();
//	}
//
//	mRTVHeapManager.Release();
//	mDSVHeapManager.Release();
//	mSRVHeapManager.Release();
//	mSamplerHeapManager.Release();
//
//	mDevice.Reset();
//	mCommandQueue.Reset();
//	mSwapChain.Reset();
//	mDXGIFactory.Reset();
//	mMainCommandList.Reset();
//	mFence.Reset();
//
//#ifdef _MEMORY_DEBUG
//	OutputDebugStringA("=================================================DX12 Debug Info=================================================\n");
//	ComPtr<IDXGIDebug> dxgiDebug;
//	HRESULT hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiDebug.GetAddressOf()));
//	if (SUCCEEDED(hr))
//	{
//		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
//		dxgiDebug.Reset();
//	}
//	OutputDebugStringA("=================================================DX12 Debug Info End=============================================\n");
//#endif
//}
//
//bool Renderer::Init()
//{
//	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
//
//	if (!SetupDevice())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup Device");
//		return false;
//	}
//
//	if (!mRTVHeapManager.Init(mDevice.Get()))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup RTV Heap Manager");
//		return false;
//	}
//
//	if (!mDSVHeapManager.Init(mDevice.Get()))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup DSV Heap Manager");
//		return false;
//	}
//
//	if (!mSRVHeapManager.Init(mDevice.Get()))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup SRV Heap Manager");
//		return false;
//	}
//
//	if (!mSamplerHeapManager.Init(mDevice.Get()))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup Sampler Heap Manager");
//		return false;
//	}
//
//	if (!SetupCommandQueue())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup commandqueue");
//		return false;
//	}
//
//	if (!SetupSwapChain(windowsApp))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup swapchain");
//		return false;
//	}
//
//	if (!SetupBackBufferRTVs())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup BackbufferRTVs");
//		return false;
//	}
//
//	if (!SetupCommandAllocatorAndMainList())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup commandallocatorAndMainList");
//		return false;
//	}
//
//	if (!SetupFence())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup fence");
//		return false;
//	}
//
//	if (!SetupDefaultRootSignatures())
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup default root signatures");
//		return false;
//	}
//
//	mMainCamera = new Camera();
//
//	windowsApp->OnWindowResizeEvent.AddRaw(this, &Renderer::ResizeBackbuffer);
//
//	return true;
//}
//
//void Renderer::PreRender()
//{
//	mDrawCalls = 0;
//
//	mMainCommandAllocators[mFrameIndex]->Reset();
//	mMainCommandList->Reset(mMainCommandAllocators[mFrameIndex].Get(), nullptr);
//
//	RenderTarget* backbufferTarget = mBackBuffers[mFrameIndex];
//
//	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
//		backbufferTarget->GetResource().Get(),
//		D3D12_RESOURCE_STATE_PRESENT,
//		D3D12_RESOURCE_STATE_RENDER_TARGET
//	);
//	mMainCommandList->ResourceBarrier(1, &barrier);
//
//	const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle = backbufferTarget->GetRTVHandle();
//	mMainCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
//
//	mMainCommandList->ClearRenderTargetView(rtvHandle, mClearColor, 0, nullptr);
//	mMainCommandList->ClearDepthStencilView(backbufferTarget->GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
//
//	mMainCommandList->RSSetViewports(1, &backbufferTarget->GetViewport());
//	mMainCommandList->RSSetScissorRects(1, &backbufferTarget->GetScissorRect());
//
//	ID3D12DescriptorHeap* heaps[] = { mSRVHeapManager.GetHeap()/*, mSamplerHeapManager.GetHeap()*/ };
//	mMainCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
//
//
//}
//
//void Renderer::RenderToBackbuffer()
//{
//	mMainCommandList->OMSetRenderTargets(1, &mBackBuffers[mFrameIndex]->GetRTVHandle(), false, nullptr);
//}
//
//void Renderer::Present()
//{
//	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
//		mBackBuffers[mFrameIndex]->GetResource().Get(),
//		D3D12_RESOURCE_STATE_RENDER_TARGET,
//		D3D12_RESOURCE_STATE_PRESENT
//	);
//	mMainCommandList->ResourceBarrier(1, &barrier);
//
//	mMainCommandList->Close();
//	ID3D12CommandList* commandLists[] = { mMainCommandList.Get() };
//	mCommandQueue->ExecuteCommandLists(1, commandLists);
//
//	mSwapChain->Present(mVsync, 0);
//
//	WaitForGPU();
//
//	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
//}
//
////TODO: look this over
//void Renderer::ProcessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_ENTERSIZEMOVE:
//	{
//		SetIsResizingBackbuffer(true);
//		break;
//	}
//	case WM_EXITSIZEMOVE:
//	{
//		SetIsResizingBackbuffer(false);
//		ResizeBackbuffer(LOWORD(lParam), HIWORD(lParam));
//		break;
//	}
//	case WM_SIZE:
//	{
//		if (!IsResizingBackbuffer() && wParam != SIZE_MINIMIZED)
//		{
//			ResizeBackbuffer(LOWORD(lParam), HIWORD(lParam));
//		}
//		break;
//	}
//	}
//}
//
//void Renderer::ResizeBackbuffer(int32 Width, int32 Height)
//{
//	if (IsResizingBackbuffer())
//	{
//		return;
//	}
//
//	if (mSwapChain != nullptr)
//	{
//		WaitForGPU();
//
//		DXGI_SWAP_CHAIN_DESC swapDesc = {};
//		mSwapChain->GetDesc(&swapDesc);
//
//		for (uint32 i = 0; i < BACKBUFFER_COUNT; i++)
//		{
//			delete mBackBuffers[i];
//			mBackBuffers[i] = nullptr;
//		}
//
//		HRESULT hr = mSwapChain->ResizeBuffers(BACKBUFFER_COUNT, Width, Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
//		if (FAILED(hr))
//		{
//			_LOG_RENDERER_ERROR("ResizeBuffers failed with error code {}", hr);
//			switch (hr) {
//			case DXGI_ERROR_INVALID_CALL:
//				_LOG_RENDERER_ERROR("DXGI_ERROR_INVALID_CALL");
//				break;
//			case DXGI_ERROR_DEVICE_REMOVED:
//				_LOG_RENDERER_ERROR("DXGI_ERROR_DEVICE_REMOVED");
//				break;
//			default:
//				_LOG_RENDERER_ERROR("Unknown error occurred.");
//				break;
//			}
//		}
//
//		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
//		if (!SetupBackBufferRTVs())
//		{
//			_LOG_RENDERER_ERROR("Failed to resize backbuffers");
//		}
//		OnBackbufferResize.Notify(Vector2(Width, Height));
//	}
//}
//
//void Renderer::SetMainCamera(Camera* Camera)
//{
//	mMainCamera = Camera;
//}
//
//Camera* Renderer::GetMainCamera()
//{
//	return mMainCamera;
//}
//
//#pragma region INTERNAL_FUNCTIONS
//
//bool Renderer::SetupDevice()
//{
//#ifdef _DEBUG 
//	ComPtr<ID3D12Debug> debugController;
//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//	{
//		debugController->EnableDebugLayer();
//	}
//#endif // _DEBUG
//
//	UINT flag = 0;
//#ifdef _MEMORY_DEBUG
//	flag = DXGI_CREATE_FACTORY_DEBUG;
//#endif // _MEMORY_DEBUG
//
//	HRESULT hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(mDXGIFactory.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup Device Factory: {}", hr);
//		return false;
//	}
//
//	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(mDevice.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup Device: {}", hr);
//		return false;
//	}
//
//	return true;
//}
//
//bool Renderer::SetupCommandQueue()
//{
//	D3D12_COMMAND_QUEUE_DESC desc = {};
//	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//
//	HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(mCommandQueue.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to setup Command Queue: {}", hr);
//		return false;
//	}
//
//	return true;
//}
//
//bool Renderer::SetupSwapChain(WindowsApplication* App)
//{
//	IWindowInfo& windowInfo = App->GetWindowInfo();
//
//	DXGI_SWAP_CHAIN_DESC1 desc = {};
//	desc.BufferCount = BACKBUFFER_COUNT;
//	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT /*| DXGI_USAGE_SHADER_INPUT*/;
//	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	desc.Stereo = FALSE;
//	desc.SampleDesc.Count = 1;
//	desc.SampleDesc.Quality = 0;
//	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // If you want to handle fullscreen mode
//
//	ComPtr<IDXGISwapChain1> tempSwapChain;
//	HRESULT hr = mDXGIFactory->CreateSwapChainForHwnd(
//		mCommandQueue.Get(),
//		App->GetWindowsWindowInfo().windowHandle,
//		&desc,
//		nullptr,
//		nullptr,
//		tempSwapChain.GetAddressOf()
//	);
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to create SwapChain: {}", hr);
//		return false;
//	}
//
//	tempSwapChain.As(&mSwapChain);
//	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
//
//	DXGI_SWAP_CHAIN_DESC swapDesc = {};
//	mSwapChain->GetDesc(&swapDesc);
//
//	windowInfo.viewportWidth = swapDesc.BufferDesc.Width;
//	windowInfo.viewportHeight = swapDesc.BufferDesc.Height;
//
//	return true;
//}
//
//bool Renderer::SetupBackBufferRTVs()
//{
//	for (uint32 i = 0; i < BACKBUFFER_COUNT; i++)
//	{
//		ID3D12Resource* backBufferResource = nullptr;
//		HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource));
//		if (FAILED(hr))
//		{
//			_LOG_RENDERER_CRITICAL("Failed to get backbuffer {}: {}", i, hr);
//			return false;
//		}
//		mBackBuffers[i] = RenderTarget::Create(backBufferResource, true);
//		backBufferResource->Release();
//	}
//
//	return true;
//}
//
//bool Renderer::SetupCommandAllocatorAndMainList()
//{
//	for (int32 i = 0; i < BACKBUFFER_COUNT; i++)
//	{
//		HRESULT hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mMainCommandAllocators[i].GetAddressOf()));
//		if (FAILED(hr))
//		{
//			_LOG_RENDERER_CRITICAL("Failed to create CommandAllocator: {}", hr);
//			return false;
//		}
//	}
//
//	HRESULT hr = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mMainCommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(mMainCommandList.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to create MainCommandList: {}", hr);
//		return false;
//	}
//	mMainCommandList->Close();
//
//	return true;
//}
//
//bool Renderer::SetupFence()
//{
//	HRESULT hr = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to create Fence: {}", hr);
//		return false;
//	}
//
//	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	return true;
//}
//
//bool Renderer::SetupDefaultRootSignatures()
//{
//	{
//		CD3DX12_STATIC_SAMPLER_DESC sampler2d;
//		sampler2d.Init(
//			0,
//			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
//			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
//			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
//			D3D12_TEXTURE_ADDRESS_MODE_WRAP
//		);
//
//		CD3DX12_DESCRIPTOR_RANGE srvRange;
//		srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
//
//		CD3DX12_ROOT_PARAMETER params[2] = { {},{} };
//		params[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL); //t0
//		params[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //b0
//
//		CD3DX12_ROOT_SIGNATURE_DESC sigDesc;
//		sigDesc.Init(
//			_countof(params),
//			params,
//			1,
//			&sampler2d,
//			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
//		);
//
//		if(!mRootSignatureManager.Create(sigDesc, "SpriteBindless"))
//		{
//			_LOG_RENDERER_ERROR("Failed to create SpriteBindless root signature");
//			return false;
//		}
//	}
//
//	return true;
//}
//
//void Renderer::WaitForGPU()
//{
//	mFenceValue++;
//	mCommandQueue->Signal(mFence.Get(), mFenceValue);
//	if (mFence->GetCompletedValue() < mFenceValue)
//	{
//		mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
//		WaitForSingleObject(mFenceEvent, INFINITE);
//	}
//}
//
//#pragma endregion
//
//void Renderer::AddDrawCall()
//{
//	mDrawCalls++;
//}
//
//void Renderer::SetIsResizingBackbuffer(bool Resize)
//{
//	mIsResizing = Resize;
//}