#include "pch.h"

#include "RendererFactory.h"
#include <DX11RenderingBackend.h>
#include <DX11RenderBufferFactory.h>

#include <AssetRegistry.h>
#include <RenderTarget/RenderTargetResourceManager.h>
#include <RenderTarget/DX11RenderTargetFactory.h>
#include <Shader/InputLayoutManager.h>
#include <DX11InputLayoutFactory.h>
#include <RenderQueue.h>
#include <DX11RenderStateManager.h>
#include <DX11ShaderFactory.h>
#include <Shader/ShaderManager.h>
#include <Texture/TextureResourceManager.h>
#include <Texture/DX11TextureFactory.h>

Renderer* RendererFactory::CreateRenderer()
{
	AssetRegistry* registry = AssetRegistry::GetInstance();
	Renderer* renderer = Renderer::GetInstance();

	registry->RegisterManager<RenderTargetResourceManager>();
	registry->RegisterManager<TextureResourceManager>();

	switch (0)
	{
	case 0:
	{

		DX11RenderingBackend* dxRenderer = new DX11RenderingBackend();
		renderer->mRenderingBackend = dxRenderer;

		DX11RenderBufferFactory* dx11bufferFactory = new DX11RenderBufferFactory();
		renderer->mRenderBufferFactory = dx11bufferFactory;


		RenderTargetResourceManager* renderTargetManager = registry->GetManager<RenderTargetResourceManager>();
		DX11RenderTargetFactory* renderTargetFactory = new DX11RenderTargetFactory();
		renderTargetManager->SetFactory(renderTargetFactory);

		DX11InputLayoutFactory* layoutFactory = new DX11InputLayoutFactory();
		InputLayoutManager* layoutManager = new InputLayoutManager(layoutFactory);
		renderer->mInputLayoutManager = layoutManager;


		ShaderManager* shaderManager = new ShaderManager();
		DX11ShaderFactory* dx11shaderFactory = new DX11ShaderFactory();
		shaderManager->SetShaderFactory(dx11shaderFactory);
		renderer->mShaderManager = shaderManager;

		TextureResourceManager* textureManager = registry->GetManager<TextureResourceManager>();
		textureManager->SetFactory<DX11TextureFactory>();

		renderer->mRenderQueue = new RenderQueue();
		DX11RenderStateManager* dx11StateManager = new DX11RenderStateManager();
		renderer->mRenderQueue->_SetStateManager(dx11StateManager);

		return renderer;
		break;
	}
	default:
		break;
	}
	return nullptr;
}
