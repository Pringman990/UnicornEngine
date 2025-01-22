#include "pch.h"
#include "DX11RenderingBackend.h"

#include <Application/Application.h>
#include <Application/Windows/WindowsApplication.h>
#include "RenderTarget/DX11RenderTarget.h"
#include "RenderTarget/DX11RenderTargetFactory.h"

//maybe remove these?
#include "Shader/MaterialShader.h"
#include "Shader/DX11PixelShaderBackend.h"
#include "Shader/DX11VertexShaderBackend.h"
#include "DX11ShaderFactory.h"
#include "Shader/ShaderManager.h"
#include "AssetRegistry.h"
#include "ConstantBuffer/DX11ConstantBuffer.h"
#include "Camera.h"

DX11RenderingBackend* DX11RenderingBackend::_sInstance = nullptr;

DX11RenderingBackend::DX11RenderingBackend()
	:
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mSamplerState(nullptr),
	mBackBuffer(nullptr),
	mClearColor(Color(1, 0, 0))
{
	if (_sInstance)
		assert(false); //A renderer already exists

	_sInstance = this;
}

DX11RenderingBackend::~DX11RenderingBackend()
{
	delete mBackBuffer;
	mBackBuffer = nullptr;

	delete mCamConstant;

#ifdef _MEMORY_DEBUG
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;
	DX11RenderingBackend::_GetInstance()->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), &debug);
	if (debug) {
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif

	mDevice.Reset();
	mDeviceContext.Reset();
	mSwapChain.Reset();
	mSamplerState.Reset();

#ifdef _DEBUG
	mAdapter3.Reset();
#endif
}

bool DX11RenderingBackend::Init()
{
	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::GetInstance()->GetApplication());

	if (!SetupDevice())
	{
		_LOG_RENDERER_CRITICAL("Failed to setup Device");
		return false;
	}

	if (!SetupSwapChain(windowsApp))
	{
		_LOG_RENDERER_CRITICAL("Failed to setup swapchain");
		return false;
	}

	if (!SetupBackBufferAndDepthBuffer(windowsApp))
	{
		_LOG_RENDERER_CRITICAL("Failed to setup backbuffer");
		return false;
	}

	if (!SetupSamplerState())
	{
		_LOG_RENDERER_CRITICAL("Failed to setup samplestate");
		return false;
	}

	{
		Camera* defualtCam = new Camera();
		Renderer::GetInstance()->SetActiveCamera(defualtCam);
		
		mCamConstant = new DX11ConstantBuffer();
		float aspect = (16.f / 9.f);
		defualtCam->SetPerspective(90, aspect, 0.001f, 1000.f);
		CameraConstantBufferData cambufferData;
		cambufferData.worldToClipMatrix = defualtCam->GetClipSpaceMatrix();
		mCamConstant->Init(sizeof(CameraConstantBufferData), &cambufferData);
	}

	return true;
}

void DX11RenderingBackend::PreRender()
{
	Renderer::GetInstance()->ResetDrawCalls();

	mDeviceContext->ClearRenderTargetView(mBackBuffer->GetRTV(), (float*)&mClearColor);
	mDeviceContext->ClearDepthStencilView(mBackBuffer->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDeviceContext->OMSetDepthStencilState(mBackBuffer->GetDSS(), 0);

#ifdef _DEBUG
	constexpr size_t mbConvert = (1024 * 1024);
	DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
	if (SUCCEEDED(mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
	{
		GraphicsCardInfo info = Renderer::GetInstance()->GetGraphicsCardInfo();
		info.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
		info.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
		info.approxFreeVideoMemory = info.totalVideoMemoryInMB - info.currentVideoMemoryUsage;
		Renderer::GetInstance()->SetGraphicsCardInfo(info);
	}
#endif // _DEBUG

	CameraConstantBufferData camBuffer;
	camBuffer.worldToClipMatrix = Renderer::GetInstance()->GetActiveCamera()->GetClipSpaceMatrix();
	mCamConstant->UpdateData(&camBuffer);
	mCamConstant->Bind(ConstantBuffers::eCameraConstantBuffer);
}

void DX11RenderingBackend::RenderToBackbuffer()
{
	mDeviceContext->OMSetRenderTargets(1, mBackBuffer->GetAdressOfRTV(), mBackBuffer->GetDSV());
}

void DX11RenderingBackend::Present()
{
	mSwapChain->Present(Renderer::GetInstance()->IsVsyncEnabled(), 0);
}

#pragma region INTERNAL_FUNCTIONS

bool DX11RenderingBackend::SetupDevice()
{
	D3D_FEATURE_LEVEL featureLevel = { D3D_FEATURE_LEVEL_11_1 };

	UINT creationFlags = 0;
#ifdef _DEBUG
#define REPORT_DX_WARNINGS
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

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
		_LOG_RENDERER_CRITICAL("Failed to setup Device: {}", hr);
		return false;
	}

	return true;
}

bool DX11RenderingBackend::SetupSwapChain(WindowsApplication* aApp)
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

	IDXGIFactory4* factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);

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
		_LOG_RENDERER_CRITICAL("Failed to create SwapChain: {}", hr);
		return false;
	}

	{
		constexpr size_t mbConvert = (1024 * 1024);
		ComPtr<IDXGIAdapter> adapter;
		if (factory->EnumAdapters(0, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
		{
			GraphicsCardInfo info = Renderer::GetInstance()->GetGraphicsCardInfo();
#ifdef _DEBUG
			if (SUCCEEDED(adapter.As(&mAdapter3)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
				if (SUCCEEDED(mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
				{
					info.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
					info.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
					info.approxFreeVideoMemory = info.currentVideoMemoryUsage - info.approxFreeVideoMemory;
				}
			}
#endif // _DEBUG

			DXGI_ADAPTER_DESC adapterDesc;
			adapter->GetDesc(&adapterDesc);

			info.name = WStringToString(std::wstring(adapterDesc.Description));
			info.vendorID = static_cast<uint32>(adapterDesc.VendorId);
			info.systemMemoryInMB = static_cast<uint32>(adapterDesc.DedicatedSystemMemory / mbConvert);
			info.sharedMemoryInMB = static_cast<uint32>(adapterDesc.SharedSystemMemory / mbConvert);

			Renderer::GetInstance()->SetGraphicsCardInfo(info);
		}

	}

	return true;
}

bool DX11RenderingBackend::SetupBackBufferAndDepthBuffer(WindowsApplication* aApp)
{
	ID3D11Texture2D* backBufferTexture;
	HRESULT result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		_com_error err(result);
		std::wstring msg = err.ErrorMessage();
		std::wstring errMsg = L"Failed to create backbuffer" + msg;
		_ENSURE_RENDERER(mBackBuffer, WStringToString(errMsg).c_str());
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

	mBackBuffer = DX11RenderTargetFactory::_CreateRenderTarget(backBufferTexture, true);
	_ENSURE_RENDERER(mBackBuffer, "Failed to create backbuffer");

	return true;
}

bool DX11RenderingBackend::SetupSamplerState()
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

ID3D11Device* DX11RenderingBackend::GetDevice()
{
	return mDevice.Get();
}

ID3D11DeviceContext* DX11RenderingBackend::GetDeviceContext()
{
	return mDeviceContext.Get();
}

IDXGISwapChain1* DX11RenderingBackend::GetSwapChain()
{
	return mSwapChain.Get();
}

DX11RenderTarget* DX11RenderingBackend::GetBackBuffer()
{
	return mBackBuffer;
}

ID3D11SamplerState* DX11RenderingBackend::GetSamplerState()
{
	return mSamplerState.Get();
}

ID3D11SamplerState** DX11RenderingBackend::GetAdressOfSamplerState()
{
	return mSamplerState.GetAddressOf();
}