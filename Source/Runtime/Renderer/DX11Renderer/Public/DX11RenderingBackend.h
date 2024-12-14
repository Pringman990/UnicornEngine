#pragma once
#include <IRenderingBackend.h>
#include <Renderer.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;
struct IDXGIAdapter3;

class WindowsApplication;
class DX11RenderTarget;


//Temp
class DX11ConstantBuffer;

class DX11RenderingBackend : public IRenderingBackend
{
public:
	DX11RenderingBackend();
	virtual ~DX11RenderingBackend() override;

	/// <summary>
	/// Internal use ONLY.
	/// Used to not having to cast from base class when used internaly
	/// Renderer could be nullptr if the engine is not using the current API.
	/// </summary>
	static DX11RenderingBackend* _GetInstance()
	{
		return _sInstance;
	}
	
	virtual bool Init() override;
	virtual void PreRender() override;
	virtual void RenderToBackbuffer() override;
	virtual void Present() override;

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain1* GetSwapChain();
	DX11RenderTarget* GetBackBuffer();
	ID3D11SamplerState* GetSamplerState();
	ID3D11SamplerState** GetAdressOfSamplerState();

private:
	bool SetupDevice();
	bool SetupSwapChain(WindowsApplication* aApp);
	bool SetupBackBufferAndDepthBuffer(WindowsApplication* aApp);
	bool SetupSamplerState();
private:
	static DX11RenderingBackend* _sInstance;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext;
	ComPtr<IDXGISwapChain1> mSwapChain;
	ComPtr<ID3D11SamplerState> mSamplerState;

	DX11RenderTarget* mBackBuffer;

	Color mClearColor;
#ifdef _DEBUG
	ComPtr<IDXGIAdapter3> mAdapter3;
#endif

	DX11ConstantBuffer* mCamConstant = nullptr; 
};