#pragma once
#include "RenderTarget.h"
#include <IResourceManager.h>

class IRenderTargetFactory;

class RenderTargetResourceManager : public IResourceManager
{
public:
	RenderTargetResourceManager();
	~RenderTargetResourceManager() override;

	RenderTarget* CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true);
	void DestroyRenderTarget(RenderTarget* aRenderTarget);

	void SetFactory(IRenderTargetFactory* aFactory);

private:
	IRenderTargetFactory* mFactory;

	std::vector<RenderTarget*> mRenderTargets;
};