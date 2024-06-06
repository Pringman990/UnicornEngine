#include "GraphicsPch.h"
#include "GraphicsEngine.h"

#include "Window.h"
#include "DX11.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "light/LightManager.h"
#include "model/ModelFactory.h"
#include "shaders/ShaderManager.h"

GraphicsEngine* GraphicsEngine::mInstance = nullptr;

GraphicsEngine::GraphicsEngine()
{
}

GraphicsEngine::~GraphicsEngine()
{

}

GraphicsEngine& GraphicsEngine::GetInstance()
{
	return *mInstance;
}

bool GraphicsEngine::Start()
{
	if (mInstance == nullptr)
	{
		mInstance = new GraphicsEngine();
		return mInstance->Init();
	}

	return false;
}

void GraphicsEngine::Shutdown()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void GraphicsEngine::PreRender()
{
	mDX11->PreRender();
}

void GraphicsEngine::Render()
{
	mDX11->Render();
}

void GraphicsEngine::PostRender()
{
	UpdateConstantBuffers();
	mDX11->PostRender();
}

WinAPI::Window& GraphicsEngine::GetCurrentWindow()
{
	return *mWindow;
}

dx::DX11& GraphicsEngine::GetDX11()
{
	return *mDX11;
}

std::shared_ptr<Camera> GraphicsEngine::GetCamera()
{
	return mCamera;
}

LightManager& GraphicsEngine::GetLightManager() const
{
	return *mLightManager;
}

ModelFactory& GraphicsEngine::GetModelFactory() const
{
	return *mModelFactory;
}

ShaderManager& GraphicsEngine::GetShaderManager() const
{
	return *mShaderManager;
}

bool GraphicsEngine::Init()
{
	{
		if (mWindow == nullptr)
			mWindow = std::make_unique<WinAPI::Window>();

		if (!mWindow->Init())
			return false;
	}
	{
		if (mDX11 == nullptr)
			mDX11 = std::make_unique<dx::DX11>();

		if (!mDX11->Init())
			return false;
	}
	{
		if (mShaderManager == nullptr)
			mShaderManager = std::make_unique<ShaderManager>();
	}
	{
		if (mCamera == nullptr)
			mCamera = std::make_shared<Camera>();

		mCamera->SetPerspective(90, 16.f / 9.f, 0.01f, 1000.f);
	}
	{
		if (mCameraConstantBuffer == nullptr)
			mCameraConstantBuffer = std::make_unique<ConstantBuffer>();

		CameraConstantBufferData cameraBuffer;
		cameraBuffer.worldToClipMatrix = mCamera->GetClipSpaceMatrix();
		cameraBuffer.position = mCamera->GetTransform().GetPosition();

		if (!mCameraConstantBuffer->Init(sizeof(CameraConstantBufferData), &cameraBuffer))
			return false;
	}
	{
		if (mLightManager == nullptr)
			mLightManager = std::make_unique<LightManager>();

		mLightManager->InitDefault();

		if (mLightConstantBuffer == nullptr)
			mLightConstantBuffer = std::make_unique<ConstantBuffer>();

		LightConstantBufferData data;
		data.numberOfMips = mLightManager->GetAmbientLight().GetNumMips();
		data.ambientLightColorAndIntensity = mLightManager->GetAmbientLight().GetColor();
		data.ambientLightColorAndIntensity.w = mLightManager->GetAmbientLight().GetIntensity();
		data.directionalLightColorAndIntensity = mLightManager->GetDirectionalLight().GetColor();
		data.directionalLightColorAndIntensity.w = mLightManager->GetDirectionalLight().GetIntensity();
		data.directionalLightDirection = Vector4(mLightManager->GetDirectionalLight().GetTransform().GetMatrix().Forward(), 0.0f);

		if (!mLightConstantBuffer->Init(sizeof(LightConstantBufferData), &data))
			return false;
	}
	{
		if (mModelFactory == nullptr)
			mModelFactory = std::make_unique<ModelFactory>();
	}

	return true;
}

void GraphicsEngine::UpdateConstantBuffers()
{
	CameraConstantBufferData cameraBuffer;
	cameraBuffer.worldToClipMatrix = mCamera->GetClipSpaceMatrix();
	mCameraConstantBuffer->Bind(ConstantBuffer::eCameraConstantBuffer);
	mCameraConstantBuffer->Update(&cameraBuffer);

	LightConstantBufferData data = {};
	data.numberOfMips = mLightManager->GetAmbientLight().GetNumMips();
	data.ambientLightColorAndIntensity = mLightManager->GetAmbientLight().GetColor();
	data.ambientLightColorAndIntensity.w = mLightManager->GetAmbientLight().GetIntensity();
	data.directionalLightColorAndIntensity = mLightManager->GetDirectionalLight().GetColor();
	data.directionalLightColorAndIntensity.w = mLightManager->GetDirectionalLight().GetIntensity();
	data.directionalLightDirection = Vector4(mLightManager->GetDirectionalLight().GetTransform().GetEularRotation(), 0.0f);
	
	uint32_t pointLightCount =  mLightManager->GetNumberOfPointLights();
	std::array<PointLight, MAX_NUMBER_OF_LIGHTS>& pointLights =  mLightManager->GetPointLights();
	for (uint32_t i = 0; i < pointLightCount; i++)
	{
		data.numPointLights = pointLightCount;
		data.pointLights[i].color = pointLights[i].GetColor();
		data.pointLights[i].color.w = pointLights[i].GetIntensity();
		data.pointLights[i].position = pointLights[i].GetTransform().GetPosition();
		data.pointLights[i].radius = pointLights[i].GetRadius();
	}
	
	mLightConstantBuffer->Bind(ConstantBuffer::eLightConstantBuffer);
	mLightConstantBuffer->Update(&data);

	if (mLightManager->GetAmbientLight().GetCubeMap())
		mDX11->GetDeviceContext()->PSSetShaderResources(0, 1, mLightManager->GetAmbientLight().GetCubeMap().GetAddressOf());
}


