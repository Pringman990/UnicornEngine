#pragma once
#include <IRenderer.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;

class WindowsApplication;
class DX11RenderTargetBackend;

class DX11Renderer : public IRenderer
{
public:
	DX11Renderer();
	virtual ~DX11Renderer() override;

	/// <summary>
	/// Internal use ONLY.
	/// Renderer could be nullptr if the engine is not using the current API
	/// </summary>
	static DX11Renderer* _GetInstance()
	{
		return _sInstance;
	}

	virtual void Shutdown() override;
	
	virtual bool Init() override;
	virtual void Update() override;

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain1* GetSwapChain();
	TSharedPtr<DX11RenderTargetBackend>& GetBackBufferRT();
	ID3D11SamplerState* GetSamplerState();
	ID3D11SamplerState** GetAdressOfSamplerState();

private:
	bool SetupDevice();
	bool SetupSwapChain(WindowsApplication* aApp);
	bool SetupBackBufferAndDepthBuffer(WindowsApplication* aApp);
	bool SetupSamplerState();
private:
	static DX11Renderer* _sInstance;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext;
	ComPtr<IDXGISwapChain1> mSwapChain;
	ComPtr<ID3D11SamplerState> mSamplerState;

	TSharedPtr<DX11RenderTargetBackend> mBackBufferRT;
};