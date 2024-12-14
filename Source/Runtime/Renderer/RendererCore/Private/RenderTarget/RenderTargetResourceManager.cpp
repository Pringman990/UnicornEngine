#include "../pch.h"
#include "RenderTarget/RenderTargetResourceManager.h"

#include "RenderTarget/IRenderTargetFactory.h"

RenderTargetResourceManager::RenderTargetResourceManager()
	:
	mFactory(nullptr)
{
}

RenderTargetResourceManager::~RenderTargetResourceManager()
{
	delete mFactory;
	mFactory = nullptr;
}

RenderTarget* RenderTargetResourceManager::CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting)
{
	_ENSURE_RENDERER((aSize.x > 0 && aSize.y > 0), "RenderTarget size needs to be bigger then (0,0)");
	_ENSURE_RENDERER(mFactory, "RenderTargetFactory not set");

	RenderTarget* renderTarget = mFactory->CreateRenderTarget(aSize, EnableDepthTesting);
	mRenderTargets.push_back(renderTarget);

	return renderTarget;
}

void RenderTargetResourceManager::DestroyRenderTarget(RenderTarget* aRenderTarget)
{
	int32 deleteIndex = -1;
	for (int32 i = 0; i < mRenderTargets.size(); i++)
	{
		if (aRenderTarget == mRenderTargets[i])
		{
			deleteIndex = i;
		}
	}
	if (deleteIndex == -1)
	{
		_LOG_RENDERER_WARNING("Trying to delete non existing rendertarget");
		return;
	}

	delete mRenderTargets[deleteIndex];
	EraseItemFromVector(mRenderTargets, aRenderTarget);
}

void RenderTargetResourceManager::SetFactory(IRenderTargetFactory* aFactory)
{
	mFactory = aFactory;
}
