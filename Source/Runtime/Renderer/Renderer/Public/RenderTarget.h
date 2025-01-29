#pragma once

class Texture2D;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Resize(Vector2 aSize) ;
	void Release();

	void Bind();
	void Clear();

	bool IsDepthTesting();

	static RenderTarget* Create(const Vector2& aSize, bool EnableDepthTesting = true);
	static RenderTarget* Create(ID3D11Texture2D* aTexture, bool EnableDepthTesting);
	static void Create(
		const Vector2& aSize,
		RenderTarget* aRenderTarget,
		D3D11_RENDER_TARGET_VIEW_DESC aRTVDesc,
		D3D11_TEXTURE2D_DESC aDepthTexture,
		D3D11_DEPTH_STENCIL_VIEW_DESC aDepthDesc,
		D3D11_DEPTH_STENCIL_DESC aDepthStencilDesc
	);

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

	Texture2D* GetTexture2D();

private:
	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11DepthStencilView> mDSV;
	ComPtr<ID3D11DepthStencilState> mDSS;
	ComPtr<ID3D11Texture2D> mDepthTexture;
	D3D11_VIEWPORT mViewport;

	bool mIsDepthTesting;
	Texture2D* mTexture2D;
};