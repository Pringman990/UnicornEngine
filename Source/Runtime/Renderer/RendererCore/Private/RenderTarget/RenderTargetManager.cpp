#include "RenderTarget/RenderTargetManager.h"

#include "RenderTarget/RenderTargetFactory.h"

RenderTargetManager::RenderTargetManager()
{
}

RenderTargetManager::~RenderTargetManager()
{
}

UniqueID RenderTargetManager::CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting)
{
	IRenderTargetBackend* renderTarget = RenderTargetFactory::CreateRenderTarget(aSize, EnableDepthTesting);
	UniqueID id = UniqueID::GenerateNewID();
	
	auto it = mRenderTargets.Find(id);
	if (it.IsValid())
		id = UniqueID::GenerateNewID();

	mRenderTargets[id] = renderTarget;

	return id;
}