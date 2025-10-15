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

GPUResourceHandle<Shader> GHandle;
DirectResourceHandle<GPUBuffer> GVertexBuffer;
DirectResourceHandle<GPUBuffer> GIndexBuffer;
DirectResourceHandle<GPUConstantBuffer> GConstantBuffer;
DirectResourceHandle<GPUConstantBuffer> GCameraConstantBuffer;
GPUResourceHandle<GPUMesh> GCube;

Camera GCamera;

void UpdateCamera();

struct ConstantTest
{
	float time = 0;
	Vector3 _pad;
} GTestConstant;

struct CameraConstant
{
	Matrix view;
	Matrix proj;
	Vector3 position;
	uint32 _pad;
};

bool RenderLoop::Init()
{
	mRenderer = SubsystemManager::Get<Renderer>();
	mRenderer->Init();
	GHandle = mRenderer->GetShaderManager()->CreateShader("shaders://Mesh_VS.hlsl", "shaders://Single_Color_Mesh_FS.hlsl");
	
	GConstantBuffer = mRenderer->GetBufferManager()->CreateConstantBuffer(sizeof(ConstantTest), nullptr);
	GCameraConstantBuffer = mRenderer->GetBufferManager()->CreateConstantBuffer(sizeof(CameraConstant), nullptr);

	Vector<Vertex> verticies = 
	{
		{{ -0.5f, -0.8f, 0, 1 }, { 1, 0, 0, 1}},
		{{ 0.0f, 0.8f, 0, 1   }, { 0, 1, 0, 1}},
		{{ 0.8f, -0.1f, 0, 1  }, { 0, 0, 1, 1}}
	};

	Vector<uint32> indices =
	{
		0, 1, 2
	};

	GIndexBuffer = mRenderer->GetBufferManager()->CreateIndex(indices);
	GVertexBuffer = mRenderer->GetBufferManager()->CreateVertex(verticies);

	GCube = BasicPrimitiveFactory::CreateCube();

	GCamera.SetPerspective(90.f, (16.f/9.f), 0.001f, 1000.f);
	GCamera.GetTransform().SetPosition({0,0,-3});

	return true;
}

void RenderLoop::BeginFrame()
{
	UpdateCamera();

	auto context = mRenderer->GetLogicalDevice().GetImmediateContext();
	
	auto backbuffer = mRenderer->GetTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBuffer());
	auto backbufferDSV = mRenderer->GetTextureManager()->GetInternalTexture(mRenderer->GetSwapChain()->GetBackBufferDSV());
	context->ClearRenderTargetView(backbuffer->rtv.Get(), Color(0.05f, 0.05f, 0.05f, 1));
	context->ClearDepthStencilView(backbufferDSV->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, backbuffer->rtv.GetAddressOf(), backbufferDSV->dsv.Get());

	mRenderer->GetSwapChain()->UpdateCardInfo();

	GTestConstant.time += GET_TIMER()->GetDeltaTime();

	mRenderer->GetBufferManager()->UpdateConstantBuffer(GConstantBuffer, &GTestConstant);
	mRenderer->GetBufferManager()->BindConstantBuffer(GConstantBuffer, 1, ShaderStage::FS | ShaderStage::VS);

	CameraConstant cConstant;
	cConstant.position = GCamera.GetPosition();
	cConstant.proj = GCamera.GetProjectionMatrix();
	cConstant.view = GCamera.GetViewMatrix();

	mRenderer->GetBufferManager()->UpdateConstantBuffer(GCameraConstantBuffer, &cConstant);
	mRenderer->GetBufferManager()->BindConstantBuffer(GCameraConstantBuffer, 0, ShaderStage::FS | ShaderStage::VS);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	Shader* shader = mRenderer->GetShaderManager()->GetInternalShader(GHandle);
	context->IASetInputLayout(shader->layout->layout.Get());
	context->VSSetShader(shader->vs.Get(), nullptr, 0);
	context->PSSetShader(shader->ps.Get(), nullptr, 0);

	GPUMesh* cubeMesh = mRenderer->GetMeshManager()->GetInternalMesh(GCube);
	
	uint32 stride = sizeof(Vertex);
	uint32 offset = 0;
	context->IASetVertexBuffers(0, 1, cubeMesh->vertexBuffer.ptr->buffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(cubeMesh->indexBuffer.ptr->buffer.Get(), ToDXFormat(RenderFormat::R32_UINT), 0);
	
	for (auto& subMesh : cubeMesh->submeshes)
	{
		context->DrawIndexed(subMesh.indexCount, subMesh.startIndex, 0);
	}
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