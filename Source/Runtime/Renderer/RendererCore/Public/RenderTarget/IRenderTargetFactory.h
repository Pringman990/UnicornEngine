#pragma once
#include "IRenderTargetBackend.h"

class IRenderTargetFactory
{
public:
	IRenderTargetFactory() {};
	virtual ~IRenderTargetFactory() {};

	virtual IRenderTargetBackend* CreateRenderTarget(Vector2 aSize, bool EnableDepthTesting = true) = 0;

protected:

};