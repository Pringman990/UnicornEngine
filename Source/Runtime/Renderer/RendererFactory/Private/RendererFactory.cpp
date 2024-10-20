#include "RendererFactory.h"
#include "DX11Renderer.h"

IRenderer* RendererFactory::CreateRenderer()
{
	switch (0)
	{
	case 0:
	{
		return new DX11Renderer();
		break;
	}
	default:
		break;
	}
	return nullptr;
}
