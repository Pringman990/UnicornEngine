#include "GraphicsPch.h"
#include "DX11.h"

#include "GraphicsEngine.h"
#include "Window.h"


dx::DX11::DX11()
	:
	mVsync(true),
	mColor(0,0,0,0)
{
	
}

dx::DX11::~DX11()
{
	mDevice.Reset();
	mDeviceContext.Reset();
	mSwapChain.Reset();
	mBackBuffer.Reset();
	mDepthBuffer.Reset();
	mDepthStencilState.Reset();
	mSamplerState.Reset();
}

bool dx::DX11::Init()
{
	if (!SetupDevice())
		return false;

	if (!SetupSwapChain())
		return false;

	if (!SetupBackBuffer())
		return false;

	if (!SetupDepthBuffer())
		return false;

	if (!SetupSamplerState())
		return false;

	return true;
}

void dx::DX11::PreRender()
{
	mDeviceContext->ClearRenderTargetView(mBackBuffer.Get(), (float*)&mColor);
	mDeviceContext->ClearDepthStencilView(mDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	mDeviceContext->OMSetRenderTargets(1, mBackBuffer.GetAddressOf(), mDepthBuffer.Get());
}

void dx::DX11::Render()
{
	mSwapChain->Present(mVsync, 0);
}

ID3D11Device* dx::DX11::GetDevice()
{
	return mDevice.Get();
}

ID3D11DeviceContext* dx::DX11::GetDeviceContext()
{
	return mDeviceContext.Get();
}

IDXGISwapChain1* dx::DX11::GetSwapChain()
{
	return mSwapChain.Get();
}

ID3D11RenderTargetView* dx::DX11::GetBackBuffer()
{
	return mBackBuffer.Get();
}

ComPtr<ID3D11RenderTargetView>& dx::DX11::GetBackBufferComPtr()
{
	return mBackBuffer;
}

ID3D11DepthStencilView* dx::DX11::GetDepthBuffer()
{
	return mDepthBuffer.Get();
}

ID3D11DepthStencilState* dx::DX11::GetDepthState()
{
	return mDepthStencilState.Get();
}

Color dx::DX11::GetBackgrundColor()
{
	return mColor;
}

bool dx::DX11::SetupDevice()
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
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create SwapChain: " << errorMessage << std::endl;
		return false;
	}

	return true;
}

bool dx::DX11::SetupSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Flags = 0;

	IDXGIFactory2* factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&factory);

	HRESULT hr = factory->CreateSwapChainForHwnd(
		mDevice.Get(),
		GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().currentWindow,
		&desc,
		nullptr,
		nullptr,
		mSwapChain.GetAddressOf()
	);

	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create SwapChain: " << errorMessage << std::endl;
		return false;
	}

	return true;
}

bool dx::DX11::SetupBackBuffer()
{
	ID3D11Texture2D* backBufferTexture;
	HRESULT result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create Back Buffer Texture: " << errorMessage << std::endl;
		return false;
	}

	result = mDevice->CreateRenderTargetView(backBufferTexture, nullptr, &mBackBuffer);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create RenderTargetView: " << errorMessage << std::endl;
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	mDeviceContext->OMSetRenderTargets(1, mBackBuffer.GetAddressOf(), nullptr);
	
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports(1, &viewport);

	WinAPI::WindowInfo info;
	info = GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo();
	info.viewportWidth = static_cast<int>(viewport.Width);
	info.viewportHeight = static_cast<int>(viewport.Height);
	GraphicsEngine::GetInstance().GetCurrentWindow().SetWindowInfo(info);

	return true;
}

bool dx::DX11::SetupDepthBuffer()
{
	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().viewportWidth;
	desc.Height = GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().viewportHeight;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT result = mDevice->CreateTexture2D(&desc, nullptr, &depthBufferTexture);
	if (FAILED(result))
	{
		std::cout << "Failed to create depth buffer" << std::endl;
		return false;
	}
	result = mDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &mDepthBuffer);
	if (FAILED(result))
	{
		std::cout << "Failed to create Depth Stencil View" << std::endl;
		return false;
	}
	depthBufferTexture->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());
	if (FAILED(result))
	{
		std::cout << "Failed to create Depth Stencil State" << std::endl;
		return false;
	}
	return true;
}

bool dx::DX11::SetupSamplerState()
{
	// Create a texture sampler state description.
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

	// Create the texture sampler state.
	HRESULT result = mDevice->CreateSamplerState(&samplerDesc, &mSamplerState);
	if (FAILED(result))
	{
		return false;
	}

	mDeviceContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	return true;
}
