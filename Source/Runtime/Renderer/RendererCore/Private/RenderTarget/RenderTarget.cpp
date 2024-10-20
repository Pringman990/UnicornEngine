#include "RenderTarget/RenderTarget.h"

#include "RenderTarget/RenderTargetManager.h"
#include "ResourceRegistry.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
}

RenderTarget RenderTarget::CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting)
{
	RenderTargetManager* renderTargetManager = ResourceRegistry::GetInstance().GetManager<RenderTargetManager>();
	
	UniqueID id = renderTargetManager->CreateRenderTarget(aSize, EnableDepthTesting);
	
	RenderTarget renderTarget;
	renderTarget.mRenderTargetID = id;
	
	return renderTarget;
}
