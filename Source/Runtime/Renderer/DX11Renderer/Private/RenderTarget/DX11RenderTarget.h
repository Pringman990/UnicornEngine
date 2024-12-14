#pragma once
#include <RenderTarget/RenderTarget.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Texture2D;

class DX11RenderTarget : public RenderTarget
{
public:
	DX11RenderTarget();
	~DX11RenderTarget();

	virtual void Resize(Vector2 aSize) override;
	void Release();

	void Bind() override;
	void Clear() override;

public:
	ID3D11RenderTargetView* GetRTV();
	ID3D11RenderTargetView** GetAdressOfRTV();
	ID3D11DepthStencilView* GetDSV();
	ID3D11DepthStencilView** GetAdressOfDSV();
	ID3D11DepthStencilState* GetDSS();
	ID3D11DepthStencilState** GetAdressOfDSS();
	ID3D11Texture2D* GetDepthTexture();
	ID3D11Texture2D** GetAdressOfDepthTexture();
	D3D11_VIEWPORT* GetViewPort();

private:
	friend class DX11RenderTargetFactory;

	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11DepthStencilView> mDSV;
	ComPtr<ID3D11DepthStencilState> mDSS;
	ComPtr<ID3D11Texture2D> mDepthTexture;
	D3D11_VIEWPORT mViewport;
};