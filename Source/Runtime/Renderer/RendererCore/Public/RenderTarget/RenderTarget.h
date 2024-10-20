#pragma once
#include <UniqueID.h>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	static RenderTarget CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true);

private:
	UniqueID mRenderTargetID;
};