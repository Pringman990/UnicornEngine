#pragma once
#include <RenderTarget/IRenderTargetFactory.h>

class DX11RenderTarget;
struct ID3D11Texture2D;
struct D3D11_RENDER_TARGET_VIEW_DESC;
struct D3D11_TEXTURE2D_DESC;
struct D3D11_DEPTH_STENCIL_VIEW_DESC;
struct D3D11_DEPTH_STENCIL_DESC;

class DX11RenderTargetFactory : public IRenderTargetFactory
{
public:
	DX11RenderTargetFactory();
	~DX11RenderTargetFactory() override;

	virtual RenderTarget* CreateRenderTarget(const Vector2& aSize, bool EnableDepthTesting = true) override;
	
	static DX11RenderTarget* _CreateRenderTarget(const Vector2& aSize, bool EnableDepthTesting = true);
	
	static void _CreateRenderTarget(
		const Vector2& aSize, 
		DX11RenderTarget* aRenderTarget, 
		D3D11_RENDER_TARGET_VIEW_DESC aRTVDesc,
		D3D11_TEXTURE2D_DESC aDepthTexture,
		D3D11_DEPTH_STENCIL_VIEW_DESC aDepthDesc,
		D3D11_DEPTH_STENCIL_DESC aDepthStencilDesc
	);

	/// <summary>
	/// Mainly used for creating the backbuffer
	/// </summary>
	static DX11RenderTarget* _CreateRenderTarget(ID3D11Texture2D* aTexture, bool EnableDepthTesting);

private:

private:

};