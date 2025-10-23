#include "SwapChain.h"

#include "LogicalDevice.h"
#include "Renderer.h"
#include "TextureManager.h"

#include <d3d11_4.h>

SwapChain::SwapChain()
{
}

SwapChain::~SwapChain()
{
}

OwnedPtr<SwapChain> SwapChain::Create(LogicalDevice& Device, WindowHandle Hwnd, DXGI_SWAP_CHAIN_DESC1 Desc)
{
	OwnedPtr<SwapChain> swapChain = MakeOwned<SwapChain>();

	IDXGIFactory4* factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);

	HRESULT hr = factory->CreateSwapChainForHwnd(
		Device.GetRaw(),
		Hwnd,
		&Desc,
		nullptr,
		nullptr,
		swapChain->mSwapChain.GetAddressOf()
	);

	if (FAILED(hr))
	{
		LOG_CRITICAL("Failed to create SwapChain: {}", hr);
		swapChain.reset();
		return nullptr;
	}

		Renderer* renderer = SubsystemManager::Get<Renderer>();
	{

		constexpr size_t mbConvert = (1024 * 1024);
		ComPtr<IDXGIAdapter> adapter;
		if (factory->EnumAdapters(0, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
		{
			GraphicsCardInformation newInfo = renderer->GetCardInfo();
#ifdef _DEBUG
			if (SUCCEEDED(adapter.As(&swapChain->mAdapter3)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
				if (SUCCEEDED(swapChain->mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
				{
					newInfo.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
					newInfo.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
					newInfo.approxFreeVideoMemory = newInfo.currentVideoMemoryUsage - newInfo.approxFreeVideoMemory;
				}
				else
				{
					LOG_ERROR("Failed to quary video memory");
				}
			}
			else
			{
				LOG_ERROR("Failed to get IDXGIAdapter3");
			}
#endif // _DEBUG

			DXGI_ADAPTER_DESC adapterDesc;
			adapter->GetDesc(&adapterDesc);

			newInfo.name = WStringToString(std::wstring(adapterDesc.Description));
			newInfo.vendorID = static_cast<uint32>(adapterDesc.VendorId);
			newInfo.systemMemoryInMB = static_cast<uint32>(adapterDesc.DedicatedSystemMemory / mbConvert);
			newInfo.sharedMemoryInMB = static_cast<uint32>(adapterDesc.SharedSystemMemory / mbConvert);

			renderer->SetCardInfo(newInfo);
		}
	}

	factory->Release();

	if (!swapChain->CreateTextures(renderer))
	{
		swapChain.reset();
		return nullptr;
	}

	return std::move(swapChain);
}

void SwapChain::UpdateCardInfo()
{
#ifdef _DEBUG
	constexpr size_t mbConvert = (1024 * 1024);
	DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
	if (SUCCEEDED(mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
	{
		GraphicsCardInformation info = SubsystemManager::Get<Renderer>()->GetCardInfo();
		info.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
		info.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
		info.approxFreeVideoMemory = info.totalVideoMemoryInMB - info.currentVideoMemoryUsage;
		SubsystemManager::Get<Renderer>()->SetCardInfo(info);
	}
#endif // _DEBUG
}

bool SwapChain::CreateTextures(Renderer* Renderer)
{
	ID3D11Texture2D* backBufferTexture;
	HRESULT result = mSwapChain->GetBuffer(0,  __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		_com_error err(result);
		std::wstring msg = err.ErrorMessage();
		std::wstring errMsg = L"Failed to create backbuffer" + msg;
		ENSURE(mBackBuffer.Invalid(), WStringToString(errMsg).c_str());
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	mBackBuffer = Renderer->GetTextureManager()->CreateTextureRenderTarget(backBufferTexture);
	mBackBufferDSV = Renderer->GetTextureManager()->CreateTexture2D(Vector2i(textureDesc.Width, textureDesc.Height), RenderFormat::D32_FLOAT, TextureBindFlags::DepthStencil);
	ENSURE(mBackBuffer, "Failed to create backbuffer");
	ENSURE(mBackBufferDSV, "Failed to create backbuffer depth stencil view");

	return true;
}
