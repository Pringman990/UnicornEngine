#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>
#include "LogicalDevice.h"
#include "GraphicsCardInformation.h"

class Sampler;
class SwapChain;

class TextureManager;
class ShaderManager;
class InputLayoutManager;
class RenderBufferManager;
class MeshManager;

/*
* Engine Subsystem
*/
class Renderer
{
	friend struct subsystem::SubsystemDescriptor;

public:
	bool Init();

	const LogicalDevice& GetLogicalDevice() const { return mDevice; };
	SwapChain* GetSwapChain() const { return mSwapChain.get(); };
	TextureManager* GetTextureManager() const { return mTextureManager.get(); };
	ShaderManager* GetShaderManager() const { return mShaderManager.get(); };
	InputLayoutManager* GetInputManager() const { return mInputManager.get(); };
	RenderBufferManager* GetBufferManager() const { return mRenderBufferManager.get(); };
	MeshManager* GetMeshManager() const { return mMeshManager.get(); };

	const GraphicsCardInformation& GetCardInfo() const { return mGraphicsCardInfo; };
	void SetCardInfo(const GraphicsCardInformation& Info) { mGraphicsCardInfo = Info; };

private:
	Renderer();
	~Renderer();
private:
	LogicalDevice mDevice;
	GraphicsCardInformation mGraphicsCardInfo;

	OwnedPtr<SwapChain> mSwapChain;
	OwnedPtr<Sampler> mSampler;

	OwnedPtr<TextureManager> mTextureManager;
	OwnedPtr<ShaderManager> mShaderManager;
	OwnedPtr<InputLayoutManager> mInputManager;
	OwnedPtr<RenderBufferManager> mRenderBufferManager;
	OwnedPtr<MeshManager> mMeshManager;
};