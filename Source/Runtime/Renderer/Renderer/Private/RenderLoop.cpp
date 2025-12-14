#include "RenderLoop.h"

#include "Renderer.h"
#include "SwapChain.h"
#include "GPUResources/GPUTextureManager.h"
#include "ShaderManager.h"
#include "RenderBufferManager.h"
#include "GPUResources/GPUMeshManager.h"
#include "GPUResources/GPUMaterialManager.h"
#include "BasicPrimitiveFactory.h"
#include "Camera.h"
#include "Input/InputMapper.h"
#include "ImageDecoder.h"
#include "MeshDecoder.h"

#include "RenderAssets/Mesh.h"
#include "RenderAssets/MeshManager.h"
#include <Assets/AssetRegistry.h>

#include "CommandList.h"

//TODO: Delete this because RenderLoop should not own anything
RenderLoop::~RenderLoop()
{
}

bool RenderLoop::Init()
{
	mRenderer = SubsystemManager::Get<Renderer>();
	mRenderer->Init();

	return true;
}

void RenderLoop::BeginFrame()
{
	mRenderer->GetSwapChain()->UpdateCardInfo();
	
	CommandList* context = mRenderer->GetFrameSetupCommandList();
	
	FrameConstantsData fConstant;
	fConstant.deltatime = GET_TIMER()->GetDeltaTime();

	context->UpdateConstantBuffer(mRenderer->GetFrameConstantsBuffer(), &fConstant);
	context->BindConstantBuffer(mRenderer->GetFrameConstantsBuffer(), (uint32)ConstantBufferBindSlots::Frame, ShaderStageBind::FS | ShaderStageBind::VS);

	Camera* mainCam = mRenderer->GetActiveCamera();
	CameraConstantsData cConstant;
	cConstant.position = mainCam->GetPosition();
	cConstant.proj = mainCam->GetProjectionMatrix();
	cConstant.view = mainCam->GetViewMatrix();

	context->UpdateConstantBuffer(mRenderer->GetCameraConstantsBuffer(), &cConstant);
	context->BindConstantBuffer(mRenderer->GetCameraConstantsBuffer(), (uint32)ConstantBufferBindSlots::Camera, ShaderStageBind::FS | ShaderStageBind::VS);

	context->ClearRenderTarget(mRenderer->GetMainRenderTarget().texture, Color(0.2f, 0.2f, 0.2f, 1.f));
	context->ClearDepthStencil(mRenderer->GetMainRenderTarget().dsv);
	context->SetRenderTargets({ mRenderer->GetMainRenderTarget().texture }, mRenderer->GetMainRenderTarget().dsv);

	context->SetViewport(mRenderer->GetMainRenderTarget().texture);
}

void RenderLoop::Execute()
{
	auto immediateContext = mRenderer->GetLogicalDevice().GetImmediateContext();

	CommandList* context = mRenderer->GetFrameSetupCommandList();
	auto list = context->Finalize();
	mRenderer->GetLogicalDevice().GetImmediateContext()->ExecuteCommandList(list, TRUE);
	context->Reset();

	auto backbuffer = mRenderer->GetGPUTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBuffer());
	auto backbufferDSV = mRenderer->GetGPUTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBufferDSV());
	immediateContext->OMSetRenderTargets(1, backbuffer->rtv.GetAddressOf(), backbufferDSV->dsv.Get());
}

void RenderLoop::EndFrame()
{
	(*mRenderer->GetSwapChain())->Present(0, 0);
}