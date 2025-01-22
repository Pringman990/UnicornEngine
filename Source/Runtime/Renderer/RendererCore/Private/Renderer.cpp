#include "pch.h"
#include "Renderer.h"

#include <Camera.h>

#include <AssetRegistry.h>
#include <MaterialResourceManager.h>
#include <MeshLoading/MeshResourceManager.h>
#include <Shader/ShaderManager.h>
#include "Texture/TextureResourceManager.h"

Renderer::Renderer()
	:
	mDrawCalls(0),
	mVsyncEnabled(false),
	mGraphicsCardInfo({}),
	mRenderingBackend(nullptr),
	mRenderBufferFactory(nullptr),
	mActiveCamera(nullptr),
	mInputLayoutManager(nullptr),
	mRenderQueue(nullptr)
{
}

Renderer::~Renderer()
{
	delete mRenderingBackend;
	mRenderingBackend = nullptr;

	delete mRenderBufferFactory;
	mRenderBufferFactory = nullptr;

	delete mInputLayoutManager;
	mInputLayoutManager = nullptr;

	delete mRenderQueue;
	mRenderQueue = nullptr;

	delete mActiveCamera;
	mActiveCamera = nullptr;
}

bool Renderer::Init()
{

	if (!mRenderingBackend->Init())
		return false;

	mShaderManager->Init();

	AssetRegistry* registry = AssetRegistry::GetInstance();
	
	registry->RegisterManager<MeshResourceManager>();
	registry->RegisterManager<MaterialResourceManager>();
	MaterialResourceManager* materialManager = registry->GetManager<MaterialResourceManager>();
	materialManager->Init();
	


	if (!mRenderQueue->Init())
		return false;

	return true;
}

void Renderer::PreRender()
{
	mRenderingBackend->PreRender();
}

void Renderer::RenderToBackbuffer()
{
	mRenderingBackend->RenderToBackbuffer();
}

void Renderer::Present()
{
	mRenderingBackend->Present();
}

uint32 Renderer::GetDrawCalls() const
{
	return mDrawCalls;
}

const GraphicsCardInfo& Renderer::GetGraphicsCardInfo() const
{
	return mGraphicsCardInfo;
}

void Renderer::SetGraphicsCardInfo(const GraphicsCardInfo& aInfo)
{
	mGraphicsCardInfo = aInfo;
}

void Renderer::AddDrawCall()
{
	mDrawCalls++;
}

void Renderer::ResetDrawCalls()
{
	mDrawCalls = 0;
}

bool Renderer::IsVsyncEnabled() const
{
	return mVsyncEnabled;
}

void Renderer::SetVsync(bool shouldVsync)
{
	mVsyncEnabled = shouldVsync;
}

IRenderingBackend* Renderer::GetRenderingBackend()
{
	return mRenderingBackend;
}

IRenderBufferFactory* Renderer::GetRenderBufferFactory()
{
	return mRenderBufferFactory;
}

InputLayoutManager* Renderer::GetInputLayoutManager()
{
	return mInputLayoutManager;
}

RenderQueue* Renderer::GetRenderQueue()
{
	return mRenderQueue;
}

ShaderManager* Renderer::GetShaderManager()
{
	return mShaderManager;
}

void Renderer::SetActiveCamera(Camera* aCamera)
{
	mActiveCamera = aCamera;
}

Camera* Renderer::GetActiveCamera()
{
	return mActiveCamera;
}