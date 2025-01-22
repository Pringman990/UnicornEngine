#pragma once
#include "RenderTarget.h"
#include <IAssetManager.h>

class IRenderTargetFactory;

class RenderTargetResourceManager : public IAssetManager<RenderTarget>
{
	friend class AssetRegistry;
public:
	virtual RenderTarget* LoadAsset() override;

	RenderTarget* CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true);
	void DestroyRenderTarget(RenderTarget* aRenderTarget);

	void SetFactory(IRenderTargetFactory* aFactory);

private:
	RenderTargetResourceManager();
	~RenderTargetResourceManager() override;
private:
	IRenderTargetFactory* mFactory;

	std::vector<RenderTarget*> mRenderTargets;
};