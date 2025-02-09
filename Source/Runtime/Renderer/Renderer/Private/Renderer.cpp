#include "pch.h"
#include "Renderer.h"

#include <Application/Application.h>
#include <Application/Windows/WindowsApplication.h>

#include "RenderTarget.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "RendererWindowsMessageObserver.h"

#include "Vertex.h"
#include "Camera.h"
#include "Texture2D.h"

Renderer::Renderer()
	:
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mSamplerState(nullptr),
	mBackBuffer(nullptr),
	mClearColor(Color(0.5f, 0.5f, 0.7f, 0)),
	mVsync(false)
{

}

Renderer::~Renderer()
{
	delete mBackBuffer;
	mBackBuffer = nullptr;

	delete mDefaultCamera;
	mDefaultCamera = nullptr;

	mCurrentCamera = nullptr;

	delete mCamConstant;
	mCamConstant = nullptr;
	delete mObjConstant;
	mObjConstant = nullptr;
	
	delete mWindowsMessageObserver;
	mWindowsMessageObserver = nullptr;

#ifdef _MEMORY_DEBUG
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;
	Renderer::GetInstance()->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), &debug);
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

bool Renderer::Init()
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

	if (!SetupRasterizerState())
	{
		_LOG_RENDERER_CRITICAL("Failed to setup rasterizerstate");
		return false;
	}

	mDefaultCamera = new Camera();
	mDefaultCamera->SetPerspective(90, (16 / 9), 0.001f, 1000.f);

	mCurrentCamera = mDefaultCamera;

	CameraConstantBufferData camBuffer;
	camBuffer.worldToClipMatrix = mDefaultCamera->GetClipSpaceMatrix();
	mCamConstant = new ConstantBuffer();
	mCamConstant->Init(sizeof(CameraConstantBufferData), &camBuffer);
	mCamConstant->Bind(ConstantBuffers::eCameraConstantBuffer);

	mObjConstant = new ConstantBuffer();
	ObjectConstantBufferData objBuffer;
	objBuffer.modelToWorld = Matrix();
	mObjConstant->Init(sizeof(ObjectConstantBufferData), &objBuffer);
	mObjConstant->Bind(ConstantBuffers::eObjectConstantBuffer);
	
	mWindowsMessageObserver = new RendererWindowsMessageObserver();
	windowsApp->AddWinProcObserver(mWindowsMessageObserver);

	mLightConstant = new ConstantBuffer();
	LightConstantBufferData lightBuffer;
	lightBuffer.directionalLightDirection = Vector4(0,1,0,1);
	lightBuffer.directionalLightColorAndIntensity = Color(1, 1,1,1.f);
	mLightConstant->Init(sizeof(LightConstantBufferData), &lightBuffer);
	mLightConstant->Bind(ConstantBuffers::eLightConstantBuffer);
	return true;
}

void Renderer::PreRender()
{
	//TODO: Add Drawcalls = 0

	mDeviceContext->ClearRenderTargetView(mBackBuffer->GetRTV(), (float*)&mClearColor);
	mDeviceContext->ClearDepthStencilView(mBackBuffer->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mDeviceContext->OMSetDepthStencilState(mBackBuffer->GetDSS(), 0);

#ifdef _DEBUG
	constexpr size_t mbConvert = (1024 * 1024);
	DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
	if (SUCCEEDED(mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
	{
		mGrapicsCardInfo.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
		mGrapicsCardInfo.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
		mGrapicsCardInfo.approxFreeVideoMemory = mGrapicsCardInfo.totalVideoMemoryInMB - mGrapicsCardInfo.currentVideoMemoryUsage;
	}
#endif // _DEBUG

	CameraConstantBufferData camBuffer;
	camBuffer.worldToClipMatrix = mCurrentCamera->GetClipSpaceMatrix();
	camBuffer.cameraPosition = mCurrentCamera->GetTransform().GetPosition();
	mCamConstant->UpdateData(&camBuffer);
	mCamConstant->Bind(ConstantBuffers::eCameraConstantBuffer);
}

void Renderer::RenderToBackbuffer()
{
	mDeviceContext->OMSetRenderTargets(1, mBackBuffer->GetAdressOfRTV(), mBackBuffer->GetDSV());
}

void Renderer::CopyTextureToBackbuffer(ID3D11Texture2D* aTexture)
{
	mDeviceContext->CopyResource(mBackBuffer->GetTexture2D()->GetTexture2D(), aTexture);
}

void Renderer::Present()
{
	mSwapChain->Present(mVsync, 0);
}

void Renderer::DisableDepthWriting()
{
	mDeviceContext->OMSetDepthStencilState(mDSSWriteZero.Get(), 1);
}

void Renderer::EnableDepthWriting()
{
	mDeviceContext->OMSetDepthStencilState(mBackBuffer->GetDSS(), 1);
}

void Renderer::ResizeBackbuffer(int32 aWidth, int32 aHeight)
{
	if (mSwapChain != nullptr && mBackBuffer != nullptr)
	{
		mBackBuffer->Release();
		mSwapChain->ResizeBuffers(0, aWidth, aHeight, DXGI_FORMAT_UNKNOWN, 0);
		WindowsApplication* application = static_cast<WindowsApplication*>(Application::GetInstance()->GetApplication());
		if (!SetupBackBufferAndDepthBuffer(application))
			return;
	}
}

void Renderer::UpdateObjectBuffer(const Matrix& aMatrix, const float3& aMinBounds, const float3& aMaxBounds)
{
	ObjectConstantBufferData objBuffer;
	objBuffer.modelToWorld = aMatrix;
	objBuffer.objectMinBounds = aMinBounds;
	objBuffer.objectMaxBounds = aMaxBounds;
	mObjConstant->UpdateData(&objBuffer);
	mObjConstant->Bind(ConstantBuffers::eObjectConstantBuffer);
}

void Renderer::SetMainCamera(Camera* aCamera)
{
	mCurrentCamera = aCamera;
}

Camera* Renderer::GetMainCamera()
{
	return mCurrentCamera;
}

#pragma region INTERNAL_FUNCTIONS

bool Renderer::SetupDevice()
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

bool Renderer::SetupSwapChain(WindowsApplication* aApp)
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
#ifdef _DEBUG
			if (SUCCEEDED(adapter.As(&mAdapter3)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
				if (SUCCEEDED(mAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo)))
				{
					mGrapicsCardInfo.totalVideoMemoryInMB = memoryInfo.Budget / mbConvert;
					mGrapicsCardInfo.currentVideoMemoryUsage = memoryInfo.CurrentUsage / mbConvert;
					mGrapicsCardInfo.approxFreeVideoMemory = mGrapicsCardInfo.currentVideoMemoryUsage - mGrapicsCardInfo.approxFreeVideoMemory;
				}
			}
#endif // _DEBUG

			DXGI_ADAPTER_DESC adapterDesc;
			adapter->GetDesc(&adapterDesc);

			mGrapicsCardInfo.name = WStringToString(std::wstring(adapterDesc.Description));
			mGrapicsCardInfo.vendorID = static_cast<uint32>(adapterDesc.VendorId);
			mGrapicsCardInfo.systemMemoryInMB = static_cast<uint32>(adapterDesc.DedicatedSystemMemory / mbConvert);
			mGrapicsCardInfo.sharedMemoryInMB = static_cast<uint32>(adapterDesc.SharedSystemMemory / mbConvert);
		}

	}

	return true;
}

bool Renderer::SetupBackBufferAndDepthBuffer(WindowsApplication* aApp)
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

	mBackBuffer = RenderTarget::Create(backBufferTexture, true);
	_ENSURE_RENDERER(mBackBuffer, "Failed to create backbuffer");

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = mDevice->CreateDepthStencilState(&depthStencilDesc, mDSSWriteZero.GetAddressOf());
	if (FAILED(result))
	{
		_LOG_RENDERER_ERROR("Failed to create mDSSWriteZero: {}", result);
		return false;
	}

	return true;
}

bool Renderer::SetupSamplerState()
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

bool Renderer::SetupRasterizerState()
{
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;               // Fill triangles
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = FALSE;             // Front face is clockwise
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;

	ID3D11RasterizerState* state = nullptr;
	HRESULT hr = mDevice->CreateRasterizerState(&rastDesc, &state);
	if (FAILED(hr))
	{
		_LOG_RENDERER_CRITICAL("Failed to setup Device: {}", hr);
		return false;
	}
	mDeviceContext->RSSetState(state);
	return true;
}

#pragma endregion

ID3D11Device* Renderer::GetDevice()
{
	return mDevice.Get();
}

ID3D11DeviceContext* Renderer::GetDeviceContext()
{
	return mDeviceContext.Get();
}

IDXGISwapChain1* Renderer::GetSwapChain()
{
	return mSwapChain.Get();
}

RenderTarget* Renderer::GetBackBuffer()
{
	return mBackBuffer;
}

ID3D11SamplerState* Renderer::GetSamplerState()
{
	return mSamplerState.Get();
}

ID3D11SamplerState** Renderer::GetAdressOfSamplerState()
{
	return mSamplerState.GetAddressOf();
}

const GraphicsCardInfo& Renderer::GetGraphicsCardInfo() const
{
	return mGrapicsCardInfo;
}

ComPtr<ID3D11Buffer> Renderer::CreateIndexBuffer(const std::vector<uint32>& aIndexArray)
{
	ComPtr<ID3D11Buffer> buffer;

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(aIndexArray.size()) * static_cast<UINT>(sizeof(uint32));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &aIndexArray[0];

	HRESULT hr = mDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, buffer.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create index buffer. HRESULT: {}", hr);
		return nullptr;
	}
	return buffer;
}

ComPtr<ID3D11Buffer> Renderer::CreateVertexBuffer(const std::vector<Vertex>& aVertexArray)
{
	ComPtr<ID3D11Buffer> buffer;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(aVertexArray.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &aVertexArray[0];

	HRESULT hr = mDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, buffer.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create vertex buffer. HRESULT: {}", hr);
		return nullptr;
	}

	return buffer;
}
