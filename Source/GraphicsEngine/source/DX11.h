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

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		IDXGISwapChain1* GetSwapChain();
		ID3D11RenderTargetView* GetBackBuffer();
		ComPtr<ID3D11RenderTargetView>& GetBackBufferComPtr();
		ID3D11DepthStencilView* GetDepthBuffer();
		ID3D11DepthStencilState* GetDepthState();

		Color GetBackgrundColor();

	private:
		bool SetupDevice();
		bool SetupSwapChain();
		bool SetupBackBuffer();
		bool SetupDepthBuffer();
		bool SetupSamplerState();
	private:
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<IDXGISwapChain1> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mBackBuffer;
		ComPtr<ID3D11DepthStencilView> mDepthBuffer;
		ComPtr<ID3D11DepthStencilState> mDepthStencilState;
		ComPtr<ID3D11SamplerState> mSamplerState;

		bool mVsync;
		Color mColor;
	};
}