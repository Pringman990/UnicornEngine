#pragma once
#include <RenderTarget/IRenderTargetBackend.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;

class DX11RenderTargetBackend : public IRenderTargetBackend
{
public:
	DX11RenderTargetBackend();
	~DX11RenderTargetBackend();

	virtual void Resize(Vector2 aSize) override;
	void Release();

public:
	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetAdressOfSRV();
	ID3D11RenderTargetView* GetRTV();
	ID3D11RenderTargetView** GetAdressOfRTV();
	ID3D11DepthStencilView* GetDSV();
	ID3D11DepthStencilView** GetAdressOfDSV();
	ID3D11DepthStencilState* GetDSS();
	ID3D11DepthStencilState** GetAdressOfDSS();
	ID3D11Texture2D* GetTexture2D();
	ID3D11Texture2D** GetAddressOfTexture2D();
	D3D11_VIEWPORT* GetViewPort();

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11DepthStencilView> mDSV;
	ComPtr<ID3D11DepthStencilState> mDSS;
	ComPtr<ID3D11Texture2D> mTexture2D;
	D3D11_VIEWPORT* mViewport;
};