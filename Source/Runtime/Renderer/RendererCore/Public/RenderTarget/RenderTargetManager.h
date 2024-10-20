#pragma once
#include "IRenderTargetBackend.h"
#include <IResourceManager.h>

#include <TUMap.h>
#include <UniqueID.h>

class RenderTargetManager : public IResourceManager
{
public:
	RenderTargetManager();
	~RenderTargetManager();

	UniqueID CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true);

private:
	static RenderTargetManager* mInstance;

	TUMap<UniqueID, IRenderTargetBackend*> mRenderTargets;
};