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
	std::shared_ptr<Camera> GetCamera();
	LightManager& GetLightManager() const;
	ModelFactory& GetModelFactory() const;
	ShaderManager& GetShaderManager() const;

private:
	GraphicsEngine();
	~GraphicsEngine();

	bool Init();
	void UpdateConstantBuffers();
private:
	static GraphicsEngine* mInstance;

	std::shared_ptr<Camera> mCamera;

	std::unique_ptr<WinAPI::Window> mWindow;
	std::unique_ptr<dx::DX11> mDX11;

	std::unique_ptr<ShaderManager> mShaderManager;
	std::unique_ptr<LightManager> mLightManager;
	std::unique_ptr<ModelFactory> mModelFactory;

	std::unique_ptr<ConstantBuffer> mCameraConstantBuffer;
	std::unique_ptr<ConstantBuffer> mLightConstantBuffer;
};