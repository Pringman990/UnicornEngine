#pragma once
#include "RenderTarget.h"

class IRenderTargetFactory
{
public:
	IRenderTargetFactory() {};
	virtual ~IRenderTargetFactory() {};

	virtual RenderTarget* CreateRenderTarget(const Vector2& aSize, bool EnableDepthTesting = true) = 0;

protected:

};