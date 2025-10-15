#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>
#include <Application/Generic/WindowHandle.h>

struct IDXGISwapChain1;
struct DXGI_SWAP_CHAIN_DESC1;
struct IDXGIAdapter3;

struct GPUTexture;

class LogicalDevice;

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	static OwnedPtr<SwapChain> Create(LogicalDevice& Device, WindowHandle Hwnd, DXGI_SWAP_CHAIN_DESC1 Desc);

	void UpdateCardInfo();

	IDXGISwapChain1* GetRaw() { return mSwapChain.Get(); }
	GPUResourceHandle<GPUTexture> GetBackBuffer() { return mBackBuffer; };
	GPUResourceHandle<GPUTexture> GetBackBufferDSV() { return mBackBufferDSV; };

public:
	IDXGISwapChain1* operator->() const noexcept
	{
		return mSwapChain.Get();
	}

private:
	bool CreateTextures(class Renderer* Renderer);

private:
	ComPtr<IDXGISwapChain1> mSwapChain;
#ifdef _DEBUG
	ComPtr<IDXGIAdapter3> mAdapter3;
#endif

	GPUResourceHandle<GPUTexture> mBackBuffer;
	GPUResourceHandle<GPUTexture> mBackBufferDSV;
};