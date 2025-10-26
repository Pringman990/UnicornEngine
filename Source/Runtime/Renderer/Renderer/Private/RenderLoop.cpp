#include "RenderLoop.h"

#include "Renderer.h"
#include "SwapChain.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "RenderBufferManager.h"
#include "MeshManager.h"
#include "BasicPrimitiveFactory.h"
#include "Camera.h"
#include "Input/InputMapper.h"
#include "ImageDecoder.h"
#include "MeshDecoder.h"

#include "MeshAsset.h"
#include "MeshAssetManager.h"
#include <Assets/AssetRegistry.h>

#include "CommandList.h"

Camera GCamera;
GPUResourceHandle<GPUMesh> GCube;
GPUResourceHandle<GPUMesh> GCube2;
Transform GTransform;
Transform GTransform2;

void UpdateCamera();

bool RenderLoop::Init()
{
	mRenderer = SubsystemManager::Get<Renderer>();
	mRenderer->Init();

	GCamera.SetPerspective(90.f, (16.f/9.f), 0.001f, 1000.f);
	GCamera.GetTransform().SetPosition({0,0,-3});
	
	AssetRegistry* assetReg = SubsystemManager::Get<AssetRegistry>();
	AssetLoadData<MeshAsset> asset = assetReg->Load<MeshAsset>("engine://Models/sm_player.fbx");
	MeshAssetManager* meshMan = assetReg->GetManager<MeshAssetManager>();
	WeakPtr<MeshAsset> meshPtr =  meshMan->GetAssetPtr(asset.handle);
	if (auto mesh = meshPtr.lock())
	{
		GCube = mesh->GetGPUMeshHandle();
	}

	AssetLoadData<MeshAsset> asset2 = assetReg->Load<MeshAsset>("engine://Models/sm_player.asset");
	MeshAssetManager* meshMan2 = assetReg->GetManager<MeshAssetManager>();
	WeakPtr<MeshAsset> meshPtr2 = meshMan2->GetAssetPtr(asset2.handle);
	if (auto mesh2 = meshPtr2.lock())
	{
		GCube2 = mesh2->GetGPUMeshHandle();
	}
	GTransform2.SetPosition({-3,0,0});
	GTransform2.SetScale(GTransform2.GetScale() * 0.01f);
	GTransform.SetScale(GTransform.GetScale() * 0.01f);
	return true;
}

void RenderLoop::BeginFrame()
{
	UpdateCamera();
	
	mRenderer->GetSwapChain()->UpdateCardInfo();
	
	CommandList* context = mRenderer->GetFrameSetupCommandList();
	
	FrameConstantsData fConstant;
	fConstant.deltatime = GET_TIMER()->GetDeltaTime();

	context->UpdateConstantBuffer(mRenderer->GetFrameConstantsBuffer(), &fConstant);
	context->BindConstantBuffer(mRenderer->GetFrameConstantsBuffer(), (uint32)ConstantBufferBindSlots::Frame, ShaderStage::FS | ShaderStage::VS);

	static Vector3 rot;
	rot.x += fConstant.deltatime;
	GTransform2.SetRotation(rot);

	CameraConstantsData cConstant;
	cConstant.position = GCamera.GetPosition();
	cConstant.proj = GCamera.GetProjectionMatrix();
	cConstant.view = GCamera.GetViewMatrix();

	context->UpdateConstantBuffer(mRenderer->GetCameraConstantsBuffer(), &cConstant);
	context->BindConstantBuffer(mRenderer->GetCameraConstantsBuffer(), (uint32)ConstantBufferBindSlots::Camera, ShaderStage::FS | ShaderStage::VS);

	context->ClearRenderTarget(mRenderer->GetSwapChain()->GetBackBuffer(), Color(0.2f, 0.2f, 0.2f, 1.f));
	context->ClearDepthStencil(mRenderer->GetSwapChain()->GetBackBufferDSV());
	context->SetRenderTargets({ mRenderer->GetSwapChain()->GetBackBuffer() }, mRenderer->GetSwapChain()->GetBackBufferDSV());

	context->SetViewport(mRenderer->GetSwapChain()->GetBackBuffer());

	if (GCube && GCube2)
	{
		mRenderer->SubmitMesh(GCube, GTransform);
		mRenderer->SubmitMesh(GCube2, GTransform2);
	}
}

void RenderLoop::Execute()
{
	auto immediateContext = mRenderer->GetLogicalDevice().GetImmediateContext();

	CommandList* context = mRenderer->GetFrameSetupCommandList();
	auto list = context->Finish();
	mRenderer->GetLogicalDevice().GetImmediateContext()->ExecuteCommandList(list, TRUE);
	context->Reset();
	
	auto backbuffer = mRenderer->GetTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBuffer());
	auto backbufferDSV = mRenderer->GetTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBufferDSV());
	immediateContext->OMSetRenderTargets(1, backbuffer->rtv.GetAddressOf(), backbufferDSV->dsv.Get());
}

void RenderLoop::EndFrame()
{
	(*mRenderer->GetSwapChain())->Present(0, 0);
}


void UpdateCamera()
{
	InputMapper* input = SubsystemManager::Get<InputMapper>();

	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		input->ReleaseMouse();
		return;
	}

	Timer* timer = GET_TIMER();

	Transform& transform = GCamera.GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = 5 * timer->GetDeltaTime();

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		realMovementSpeed = 10 * timer->GetDeltaTime();
	}

	if (GetAsyncKeyState('W'))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (GetAsyncKeyState('S'))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (GetAsyncKeyState('D'))
	{
		position = position + (right * realMovementSpeed);
	}
	if (GetAsyncKeyState('A'))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (GetAsyncKeyState('E'))
	{
		position = position + (up * realMovementSpeed);
	}
	if (GetAsyncKeyState('Q'))
	{
		position = position + (up * -realMovementSpeed);
	}

	Vector3 currentRotation = transform.GetEularRotation();
	Vector3 targetRotation = currentRotation;
	Vector2 dPos = input->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = 10 * timer->GetDeltaTime();

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	float lerpSpeed = 30.0f;
	Vector3 smoothedRotation = {};
	smoothedRotation.x = std::lerp(currentRotation.x, targetRotation.x, lerpSpeed * timer->GetDeltaTime());
	smoothedRotation.y = std::lerp(currentRotation.y, targetRotation.y, lerpSpeed * timer->GetDeltaTime());
	smoothedRotation.z = 0.0f;
	transform.SetRotation(targetRotation);

	transform.SetPosition(position);
	input->CaptureMouse();
}