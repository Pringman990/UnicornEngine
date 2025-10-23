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
class MaterialManager;

class CommandList;

struct GPUConstantBuffer;
struct GPUMesh;
struct Material;

enum class ConstantBufferBindSlots : uint32
{
	Frame = 0,
	Camera = 1,
	Object = 2
};

struct GPU_ALIGNED FrameConstantsData
{
	float deltatime;
	float _pad[3];
};

struct GPU_ALIGNED CameraConstantsData
{
	Matrix view;
	Matrix proj;
	Vector3 position;
	int32 _pad0;
};

struct GPU_ALIGNED ObjectConstantBufferData
{
	Matrix modelToWorld;
};

/*
* Engine Subsystem
*/
class Renderer
{
	friend struct subsystem::SubsystemDescriptor;

public:
	bool Init();

	void SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom);
	void SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom, Vector<AssetHandle<Material>> OverrideMaterials);

	inline const LogicalDevice& GetLogicalDevice() const { return mDevice; };
	inline SwapChain* GetSwapChain() const { return mSwapChain.get(); };
	inline TextureManager* GetTextureManager() const { return mTextureManager.get(); };
	inline ShaderManager* GetShaderManager() const { return mShaderManager.get(); };
	inline InputLayoutManager* GetInputManager() const { return mInputManager.get(); };
	inline RenderBufferManager* GetBufferManager() const { return mRenderBufferManager.get(); };
	inline MeshManager* GetMeshManager() const { return mMeshManager.get(); };
	inline MaterialManager* GetMaterialManager() const { return mMaterialManager.get(); };

	inline const GraphicsCardInformation& GetCardInfo() const { return mGraphicsCardInfo; };
	void SetCardInfo(const GraphicsCardInformation& Info) { mGraphicsCardInfo = Info; };

	inline DirectResourceHandle<GPUConstantBuffer> GetFrameConstantsBuffer() const { return mFrameConstantsBuffer; }
	inline DirectResourceHandle<GPUConstantBuffer> GetCameraConstantsBuffer() const { return mCameraConstantsBuffer; }
	inline DirectResourceHandle<GPUConstantBuffer> GetObjectConstantBuffer() const { return mObjectConstantBuffer; }

	inline CommandList* GetFrameSetupCommandList() const { return mFrameSetupCommandList; };

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
	OwnedPtr<MaterialManager> mMaterialManager;

	DirectResourceHandle<GPUConstantBuffer> mFrameConstantsBuffer;
	DirectResourceHandle<GPUConstantBuffer> mCameraConstantsBuffer;
	DirectResourceHandle<GPUConstantBuffer> mObjectConstantBuffer;

	CommandList* mFrameSetupCommandList;
};