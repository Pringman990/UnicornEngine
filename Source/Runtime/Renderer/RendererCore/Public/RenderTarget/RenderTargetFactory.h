#pragma once
#include "IRenderTargetBackend.h"
#include "IRenderTargetFactory.h"

class RenderTargetFactory
{
public:
	static void SetFactory(IRenderTargetFactory* aFactory)
	{
		sFactory = aFactory;
	}

	static IRenderTargetBackend* CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true)
	{
		if (!sFactory)
		{
			LOG_RENDERER_CRITICAL("No Render Target Factory Has Been Set");
			return nullptr;
		}

		IRenderTargetBackend* renderTarget = sFactory->CreateRenderTarget(aSize, EnableDepthTesting);
		return renderTarget;
	}
private:
	RenderTargetFactory() {};
	~RenderTargetFactory() {};
private:
	static IRenderTargetFactory* sFactory;
};