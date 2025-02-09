#pragma once
#include "RenderTarget.h"
#include "Texture2D.h"

class GBuffer
{
	enum RenderTargetType
	{
		eAlbedo,
		eNormal,
		eWorldPosition,

		eCount
	};
public:
	GBuffer();
	~GBuffer();

	static std::shared_ptr<GBuffer> Create(const Vector2& aSize);
	void BindRenderTargets();
	void UnbindRenderTargets();
	void ClearRenderTargets();
	void BindTexturesToPS(uint32 aStartSlot);
	void BindTexturesToCS(uint32 aStartSlot);
	void UnbindTexturesFromCS(uint32 aStartSlot);

	Texture2D* GetAlbedoTexture();
	Texture2D* GetNormalTexture();
	Texture2D* GetWorldPositionTexture();

private:
	std::array<ComPtr<ID3D11RenderTargetView>, RenderTargetType::eCount> mRenderTargetViews;
	ComPtr<ID3D11DepthStencilState> mDSS;
	ComPtr<ID3D11DepthStencilView> mDSV;
	std::array<Texture2D*, RenderTargetType::eCount> mTextures;
	D3D11_VIEWPORT mViewport;
};