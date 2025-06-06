#pragma once
#include <Core.h>
#include <EngineSubsystem.h>

#include "DescriptorHeap.h"
#include "RootSignatureManager.h"
#include "Camera.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define BACKBUFFER_COUNT 2

//Don't forget to change in CommonMacros.hlsli aswell
#define SRV_HEAP_SIZE 1000

struct ID3D12Device;
struct IDXGISwapChain4;
struct IDXGIFactory4;
struct ID3D12DescriptorHeap;
struct ID3D12CommandQueue;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

class WindowsApplication;
class RenderTarget;
class Camera;

class RendererWindowsMessageObserver;

struct GraphicsCardInfo
{
	String name = "";
	uint32 vendorID = 0;
	uint64 totalVideoMemoryInMB = 0;	//VRAM
	uint32 systemMemoryInMB = 0;		//For integrated GPU, the dedicated RAM memory for GPU
	uint32 sharedMemoryInMB = 0;		//Fallback RAM memory for the GPU if it runs out of VRAM
	uint64 currentVideoMemoryUsage = 0;
	uint64 approxFreeVideoMemory = 0;
};

class Renderer : public EngineSubsystem<Renderer>
{
	friend class EngineSubsystem<Renderer>;
public:
	bool Init();
	void PreRender();
	void RenderToBackbuffer();
	void Present();
	void ResizeBackbuffer(int32 Width, int32 Height);

	void SetMainCamera(Camera* Camera);
	Camera* GetMainCamera();

	void AddDrawCall();
	void SetIsResizingBackbuffer(bool Resize);
	
	void WaitForGPU();

	ID3D12Device* GetDevice() { return mDevice.Get(); };
	IDXGISwapChain4* GetSwapChain() { return mSwapChain.Get(); };
	ID3D12GraphicsCommandList* GetMainCommandList() { return mMainCommandList.Get(); };
	ID3D12CommandQueue* GetCommandQueue() { return mCommandQueue.Get(); };
	
	DescriptorHeap& GetRTVHeapManager() { return mRTVHeapManager; };
	DescriptorHeap& GetDSVHeapManager() { return mDSVHeapManager; };
	DescriptorHeap& GetSRVHeapManager() { return mSRVHeapManager; };
	DescriptorHeap& GetSamplerHeapManager() { return mSamplerHeapManager; };

	RootSignatureManager& GetRootSignatureManager() { return mRootSignatureManager; };

	bool IsResizingBackbuffer() { return mIsResizing; };
	uint32 GetDrawCalls() { return mDrawCalls; };

public:
	MultiNotifierArgs<Vector2> OnBackbufferResize;

private:
	Renderer();
	~Renderer();

	void ProcessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool SetupDevice();
	bool SetupCommandQueue();
	bool SetupSwapChain(WindowsApplication* App);
	bool SetupBackBufferRTVs();
	bool SetupCommandAllocatorAndMainList();
	bool SetupFence();
	bool SetupDefaultRootSignatures();
private:
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<IDXGISwapChain4> mSwapChain;
	ComPtr<IDXGIFactory4> mDXGIFactory;

	RenderTarget* mBackBuffers[BACKBUFFER_COUNT]; //Store D3D12_CPU_DESCRIPTOR_HANDLE with the rtv

	ComPtr<ID3D12CommandAllocator> mMainCommandAllocators[BACKBUFFER_COUNT];
	ComPtr<ID3D12GraphicsCommandList> mMainCommandList; //TODO:Change to commandlist class

	ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent;
	uint64 mFenceValue;
	uint32 mFrameIndex;

	DescriptorHeap mRTVHeapManager;
	DescriptorHeap mDSVHeapManager;
	DescriptorHeap mSRVHeapManager;
	DescriptorHeap mSamplerHeapManager;

	Color mClearColor;
	bool mVsync;
	uint32 mDrawCalls;
	bool mIsResizing;

	Camera* mMainCamera;
	D3D12_CPU_DESCRIPTOR_HANDLE mDefaultSamplerCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE mDefaultSamplerGPU;

	RootSignatureManager mRootSignatureManager;
};