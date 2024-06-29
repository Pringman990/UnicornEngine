#pragma once
#include "GraphicsDefines.h"

class ConstantBuffer;
class Camera;
class LightManager;
class ModelFactory;
class ShaderManager;

namespace dx
{
	class DX11;
}

namespace WinAPI
{
	class Window;
}

class GraphicsEngine
{
public:
	static GraphicsEngine& GetInstance();

	static bool Start();
	static void Shutdown();

	void PreRender();
	void Render();
	void PostRender();

	WinAPI::Window& GetCurrentWindow();
	dx::DX11* GetDX11();
	
	void SetSpriteRenderCameraAsActive();
	std::shared_ptr<Camera> GetActiveCamera();
	void SetActiveCamera(std::shared_ptr<Camera> aCamera);

	LightManager& GetLightManager() const;
	ModelFactory& GetModelFactory() const;
	ShaderManager& GetShaderManager() const;

	const uint32_t GetDrawCalls() const;
	void AddDrawCall();

private:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init();
	void UpdateConstantBuffers();
	void UpdateCameraConstantBuffer();
private:
	static GraphicsEngine* mInstance;

	std::shared_ptr<Camera> mActiveCamera;
	std::shared_ptr<Camera> mSpriteRenderCamera;

	std::unique_ptr<WinAPI::Window> mWindow;
	std::unique_ptr<dx::DX11> mDX11;

	std::unique_ptr<ShaderManager> mShaderManager;
	std::unique_ptr<LightManager> mLightManager;
	std::unique_ptr<ModelFactory> mModelFactory;

	std::unique_ptr<ConstantBuffer> mCameraConstantBuffer;
	std::unique_ptr<ConstantBuffer> mLightConstantBuffer;

	uint32_t mDrawCalls;
};