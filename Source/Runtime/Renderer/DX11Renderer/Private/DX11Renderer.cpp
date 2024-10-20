#include "pch.h"
#include "DX11Renderer.h"

#include <Application.h>
#include <Windows/WindowsApplication.h>
#include "DX11RenderTargetBackend.h"

DX11Renderer* DX11Renderer::_sInstance = nullptr;

DX11Renderer::DX11Renderer()
	:
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mSamplerState(nullptr)
{
	if (_sInstance)
		assert(false); //A renderer already exists

	_sInstance = this;
}

DX11Renderer::~DX11Renderer()
{
}

void DX11Renderer::Shutdown()
{
}

bool DX11Renderer::Init()
{
	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::GetApp());

	if (!SetupDevice())
	{
		LOG_RENDERER_CRITICAL("Failed to setup Device");
		return false;
	}

	if (!SetupSwapChain(windowsApp))
	{
		LOG_RENDERER_CRITICAL("Failed to setup swapchain");
		return false;
	}

	if (!SetupBackBufferAndDepthBuffer(windowsApp))
	{
		LOG_RENDERER_CRITICAL("Failed to setup backbuffer");
		return false;
	}

	if (!SetupSamplerState())
	{
		LOG_RENDERER_CRITICAL("Failed to setup samplestate");
		return false;
	}

	return true;
}

void DX11Renderer::Update()
{
}

ID3D11Device* DX11Renderer::GetDevice()
{
	return mDevice.Get();
}

ID3D11DeviceContext* DX11Renderer::GetDeviceContext()
{
	return mDeviceContext.Get();
}

IDXGISwapChain1* DX11Renderer::GetSwapChain()
{
	return mSwapChain.Get();
}

TSharedPtr<DX11RenderTargetBackend>& DX11Renderer::GetBackBufferRT()
{
	return mBackBufferRT;
}

ID3D11SamplerState* DX11Renderer::GetSamplerState()
{
	return mSamplerState.Get();
}

ID3D11SamplerState** DX11Renderer::GetAdressOfSamplerState()
{
	return mSamplerState.GetAddressOf();
}

#pragma region INTERNAL_FUNCTIONS

bool DX11Renderer::SetupDevice()
{
	D3D_FEATURE_LEVEL featureLevel = { D3D_FEATURE_LEVEL_11_1 };

	UINT creationFlags = 0;
#ifdef REPORT_DX_WARNINGS
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // REPORT_DX_WARNINGS

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&mDevice,
		&featureLevel,
		&mDeviceContext
	);

	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_CRITICAL("Failed to setup Device: {}", msg);
		return false;
	}

	return true;
}

bool DX11Renderer::SetupSwapChain(WindowsApplication* aApp)
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Flags = 0;

	IDXGIFactory2* factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&factory);

	HWND windowHandle = aApp->GetWindowsWindowInfo().windowHandle;

	HRESULT hr = factory->CreateSwapChainForHwnd(
		mDevice.Get(),
		windowHandle,
		&desc,
		nullptr,
		nullptr,
		mSwapChain.GetAddressOf()
	);

	if (FAILED(hr))
	{
		_com_error err(hr);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_CRITICAL("Failed to create SwapChain: {}", msg);
		return false;
	}

	return true;
}

bool DX11Renderer::SetupBackBufferAndDepthBuffer(WindowsApplication* aApp)
{
	ID3D11Texture2D* backBufferTexture;
	HRESULT result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		_com_error err(result);
		FString msg = err.ErrorMessage();
		LOG_RENDERER_CRITICAL("Failed to create Back Buffer Texture: {}", msg);
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports(1, &viewport);

	aApp->GetWindowInfo().viewportHeight = textureDesc.Height;
	aApp->GetWindowInfo().viewportWidth = textureDesc.Width;

	//if (!mBackBufferRT->Create(backBufferTexture))
	//	return false;

	return true;
}

bool DX11Renderer::SetupSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = mDevice->CreateSamplerState(&samplerDesc, &mSamplerState);
	if (FAILED(result))
	{
		return false;
	}

	mDeviceContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	return true;
}

#pragma endregion