#pragma once
#include <IRenderer.h>
#include <TSharedPtr.h>

class RendererFactory
{
public:
	static IRenderer* CreateRenderer();

private:
	RendererFactory() { };
	~RendererFactory() {};
};