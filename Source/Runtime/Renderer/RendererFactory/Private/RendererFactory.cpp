#include "pch.h"

#include "RendererFactory.h"
#include <DX11RenderingBackend.h>
#include <DX11RenderBufferFactory.h>

#include <ResourceRegistry.h>
#include <RenderTarget/RenderTargetResourceManager.h>
#include <RenderTarget/DX11RenderTargetFactory.h>
#include <Shader/InputLayoutManager.h>
#include <DX11InputLayoutFactory.h>
#include <RenderQueue.h>
#include <DX11RenderStateManager.h>
#include <DX11ShaderFactory.h>
#include <Shader/ShaderResourceManager.h>

Renderer* RendererFactory::CreateRenderer()
{
	ResourceRegistry* registry = ResourceRegistry::GetInstance();

	switch (0)
	{
	case 0:
	{
		DX11RenderingBackend* dxRenderer = new DX11RenderingBackend();
		Renderer::GetInstance()->mRenderingBackend = dxRenderer;

		DX11RenderBufferFactory* dx11bufferFactory = new DX11RenderBufferFactory();
		Renderer::GetInstance()->mRenderBufferFactory = dx11bufferFactory;

		RenderTargetResourceManager* RenderTargetManager = new RenderTargetResourceManager();
		DX11RenderTargetFactory* renderTargetFactory = new DX11RenderTargetFactory();
		
		DX11InputLayoutFactory* layoutFactory = new DX11InputLayoutFactory();
		InputLayoutManager* layoutManager = new InputLayoutManager(layoutFactory);
		Renderer::GetInstance()->mInputLayoutManager = layoutManager;

		RenderTargetManager->SetFactory(renderTargetFactory);
		registry->RegisterManager<RenderTargetResourceManager>(RenderTargetManager);

		ShaderResourceManager* shaderRSManager = new ShaderResourceManager();
		DX11ShaderFactory* dx11shaderFactory = new DX11ShaderFactory();
		shaderRSManager->SetShaderFactory(dx11shaderFactory);
		registry->RegisterManager<ShaderResourceManager>(shaderRSManager);

		Renderer::GetInstance()->mRenderQueue = new RenderQueue();
		DX11RenderStateManager* dx11StateManager = new DX11RenderStateManager();
		Renderer::GetInstance()->mRenderQueue->_SetStateManager(dx11StateManager);

		return Renderer::GetInstance();
		break;
	}
	default:
		break;
	}
	return nullptr;
}
