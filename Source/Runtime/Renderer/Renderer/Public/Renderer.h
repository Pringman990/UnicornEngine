#pragma once
#include <Singleton.h>
#include <StandardTypes/StandardTypes.h>
#include "DescriptorHeapManager.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define BACKBUFFER_COUNT 2

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
	std::string name = "";
	uint32 vendorID = 0;
	uint64 totalVideoMemoryInMB = 0;	//VRAM
	uint32 systemMemoryInMB = 0;		//For integrated GPU, the dedicated RAM memory for GPU
	uint32 sharedMemoryInMB = 0;		//Fallback RAM memory for the GPU if it runs out of VRAM
	uint64 currentVideoMemoryUsage = 0;
	uint64 approxFreeVideoMemory = 0;
};


class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>;
public:
	bool Init();
	void PreRender();
	void RenderToBackbuffer();
	void Present();
	void ResizeBackbuffer(int32 aWidth, int32 aHeight);

	void SetMainCamera(Camera* aCamera);

	ID3D12Device* GetDevice();
	IDXGISwapChain4* GetSwapChain();
	ID3D12GraphicsCommandList* GetMainCommandList();
	ID3D12CommandQueue* GetCommandQueue();

	void AddDrawCall();
	uint32 GetDrawCalls();
	void SetIsResizingBackbuffer(bool shouldResize);
	bool IsResizingBackbuffer();

	DescriptorHeapManager& GetRTVHeapManager();
	DescriptorHeapManager& GetDSVHeapManager();
	DescriptorHeapManager& GetSRVHeapManager();
	DescriptorHeapManager& GetSamplerHeapManager();

	void WaitForGPU();
private:
	Renderer();
	~Renderer();

	bool SetupDevice();
	bool SetupCommandQueue();
	bool SetupSwapChain(WindowsApplication* aApp);
	bool SetupBackBufferRTVs();
	bool SetupCommandAllocatorAndMainList();
	bool SetupFence();
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
	uint64 mFenceValue = 0;
	uint32 mFrameIndex;

	DescriptorHeapManager mRTVHeapManager;
	DescriptorHeapManager mDSVHeapManager;
	DescriptorHeapManager mSRVHeapManager;
	DescriptorHeapManager mSamplerHeapManager;
	
	RendererWindowsMessageObserver* mWindowsMessageObserver;

	Color mClearColor;
	bool mVsync;
	uint32 mDrawCalls;
	bool mIsResizing;
};