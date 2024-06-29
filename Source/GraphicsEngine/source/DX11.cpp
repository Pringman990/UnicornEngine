#include "GraphicsPch.h"
#include "DX11.h"

#include "GraphicsEngine.h"
#include "Window.h"
#include "RenderTarget.h"

dx::DX11::DX11()
	:
	mVsync(true),
	mColor(0, 0, 0, 0),
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mSamplerState(nullptr), 
	mBackBufferRT(nullptr)
{

}

dx::DX11::~DX11()
{
	mDevice.Reset();
	mDeviceContext.Reset();
	mSwapChain.Reset();
	mSamplerState.Reset();
}

bool dx::DX11::Init()
{
	if (!SetupDevice())
		return false;

	if (!SetupSwapChain())
		return false;

	if (mBackBufferRT == nullptr)
		mBackBufferRT = std::make_shared<RenderTarget>();

	if (!SetupBackBufferAndDepthBuffer())
		return false;

	if (!SetupSamplerState())
		return false;

	if (!SetupBlendState())
		return false;

	return true;
}

void dx::DX11::PreRender()
{
	mDeviceContext->ClearRenderTargetView(mBackBufferRT->GetRTV(), (float*)&mColor);
	mDeviceContext->ClearDepthStencilView(mBackBufferRT->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDeviceContext->OMSetDepthStencilState(mBackBufferRT->GetDSS(), 0);
	mBackBufferRT->SetAsActiveRenderTarget(mBackBufferRT->GetDSV());
}

void dx::DX11::Render()
{
	mBackBufferRT->SetAsActiveRenderTarget(mBackBufferRT->GetDSV());
}

void dx::DX11::PostRender()
{
	mSwapChain->Present(mVsync, 0);
}

void dx::DX11::ResizeBackBuffer(HWND /*aWindowHandle*/, int32_t width, int32_t height)
{
	if (mSwapChain != nullptr && mBackBufferRT != nullptr)
	{
		mBackBufferRT->Release();
		mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

		if (!SetupBackBufferAndDepthBuffer())
			return;
	}
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
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
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

bool dx::DX11::SetupBackBufferAndDepthBuffer()
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

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	WinAPI::WindowInfo info;
	info = GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo();

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(textureDesc.Width);
	viewport.Height = static_cast<float>(textureDesc.Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports(1, &viewport);

	info.resolution = { viewport.Width, viewport.Height };
	GraphicsEngine::GetInstance().GetCurrentWindow().SetWindowInfo(info);


	if (!mBackBufferRT->Create(backBufferTexture))
		return false;

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

bool dx::DX11::SetupBlendState()
{
	HRESULT result = S_OK;
	D3D11_BLEND_DESC blendStateDescription = {};

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = mDevice->CreateBlendState(&blendStateDescription, mBlendStates[(int)BlendState::eDisableBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = mDevice->CreateBlendState(&blendStateDescription, mBlendStates[(int)BlendState::eAlphaBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = mDevice->CreateBlendState(&blendStateDescription, mBlendStates[(int)BlendState::eAdditiveBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	SetBlendState(eDisableBlend);

	return true;
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

std::shared_ptr<RenderTarget> dx::DX11::GetBackBuffer()
{
	return mBackBufferRT;
}

ID3D11SamplerState* dx::DX11::GetSamplerState()
{
	return mSamplerState.Get();
}

ID3D11SamplerState** dx::DX11::GetAdressOfSamplerState()
{
	return mSamplerState.GetAddressOf();
}

Color dx::DX11::GetBackgrundColor()
{
	return mColor;
}

void dx::DX11::SetBlendState(BlendState aState)
{
	mCurrentBlendState = aState;
	mDeviceContext->OMSetBlendState(mBlendStates[mCurrentBlendState].Get(), nullptr, 0xffffffff);
}
