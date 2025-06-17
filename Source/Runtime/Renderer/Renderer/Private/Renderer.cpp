#include "Renderer.h"

#include "Vulkan/VkRHIRenderer.h"

#include "Texture2DLoader.h"

Renderer::Renderer()
	:
	mDrawCalls(0),
	mVsync(false),
	mActiveCamera(nullptr),
	mRHIRenderer(nullptr)
{

}

Renderer::~Renderer()
{

}

bool Renderer::Init()
{
	AssetManager::Get()->RegisterLoader<Texture2D>(new Texture2DLoader());
	AssetManager::Get()->RegisterPool<Texture2D>(new ResourcePool<Texture2D>());

	if (!CreateRHI())
	{
		_LOG_RENDERER_CRITICAL("No supported rendering API for current OS");
		return false;
	}

	if (!mRHIRenderer->Init())
	{
		_LOG_RENDERER_CRITICAL("Failed to Init RHI Renderer");
		return false;
	}
	return true;
}

void Renderer::PreRender()
{
	mRHIRenderer->PreRender();
}

void Renderer::Present()
{
	mRHIRenderer->Present();
}

bool Renderer::CreateRHI()
{
#ifdef _Win32
	mRHIRenderer = MakeOwned<VkRHIRenderer>();
	return true;
#else
	return false;
#endif
}