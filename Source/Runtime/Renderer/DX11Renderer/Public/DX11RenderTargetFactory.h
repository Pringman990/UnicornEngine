#pragma once
#include <RenderTarget/IRenderTargetFactory.h>

class DX11RenderTargetBackend;

class DX11RenderTargetFactory : public IRenderTargetFactory
{
public:
	DX11RenderTargetFactory();
	~DX11RenderTargetFactory() override;

	virtual IRenderTargetBackend* CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true) override;
	
	static DX11RenderTargetBackend* _CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true);
	static void _CreateRenderTarget(Vector2 aSize, DX11RenderTargetBackend* aRenderTarget);
	static DX11RenderTargetBackend* _CreateRenderTarget(ID3D11Texture2D* aTexture2D);

private:

private:

};