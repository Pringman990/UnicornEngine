#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	bool Create(Vector2 aSize, bool EnableDepthTesting = true);
	bool Create(ID3D11Texture2D* aTexture2D);
	void SetAsActiveRenderTarget(ID3D11DepthStencilView* aDSV = nullptr);
	void Clear();

	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetAdressOfSRV();
	ID3D11RenderTargetView* GetRTV();
	ID3D11RenderTargetView** GetAdressOfRTV();
	ID3D11DepthStencilView* GetDSV();
	ID3D11DepthStencilState* GetDSS();
	ID3D11Texture2D* GetTexture2D();

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11DepthStencilView> mDSV;
	ComPtr<ID3D11DepthStencilState> mDSS;
	ComPtr<ID3D11Texture2D> mTexture2D;
	D3D11_VIEWPORT mViewport;

	Vector2 mSize;
};