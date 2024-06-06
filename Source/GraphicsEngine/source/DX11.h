#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;

class RenderTarget;

namespace dx
{
	class DX11
	{
	public:
		DX11();
		~DX11();

		bool Init();
		void PreRender();
		void Render();
		void PostRender();

		void ResizeBackBuffer(HWND aWindowHandle, int32_t width, int32_t height);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		IDXGISwapChain1* GetSwapChain();
		std::shared_ptr<RenderTarget> GetBackBuffer();
		std::shared_ptr<RenderTarget> GetOffscreenRT();
		ID3D11SamplerState* GetSamplerState();
		ID3D11SamplerState** GetAdressOfSamplerState();

		Color GetBackgrundColor();

	private:

		bool SetupDevice();
		bool SetupSwapChain();
		bool SetupBackBufferAndDepthBuffer();
		bool SetupSamplerState();
	private:
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<IDXGISwapChain1> mSwapChain;
		ComPtr<ID3D11SamplerState> mSamplerState;

		std::shared_ptr<RenderTarget> mBackBufferRT;

		bool mVsync;
		Color mColor;
	};
}