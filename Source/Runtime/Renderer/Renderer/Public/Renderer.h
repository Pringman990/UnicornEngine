#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>
#include "LogicalDevice.h"
#include "GraphicsCardInformation.h"
#include <Assets/AssetRef.h>

class Sampler;
class SwapChain;

class ShaderManager;
class InputLayoutManager;
class RenderBufferManager;

class GPUTextureManager;
class GPUMeshManager;
class GPUMaterialManager;

class CommandList;

struct GPUConstantBuffer;
struct GPUMesh;
struct GPUTexture;

class Material;
class Mesh;

//TODO: Add Camera manager
class Camera;

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

struct MainRenderTarget
{
	//Combined srv and rtv
	GPUResourceHandle<GPUTexture> texture;
	GPUResourceHandle<GPUTexture> dsv;
};

/*
* Engine Subsystem
*/
class Renderer
{
	friend struct subsystem::SubsystemDescriptor;

public:
	bool Init();

	void SubmitMesh(AssetRef<Mesh> Mesh, const Transform& ObjectTransfrom);
	void SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom, Vector<Material*> OverrideMaterials);

	void HandleResizeEvent(int32 Width, int32 Height);

	inline const LogicalDevice& GetLogicalDevice() const { return mDevice; };
	inline SwapChain* GetSwapChain() const { return mSwapChain.get(); };
	
	inline ShaderManager* GetShaderManager() const { return mShaderManager.get(); };
	inline InputLayoutManager* GetInputManager() const { return mInputManager.get(); };
	inline RenderBufferManager* GetBufferManager() const { return mRenderBufferManager.get(); };
	
	inline GPUTextureManager*	GetGPUTextureManager()	const { return mTextureManager.get(); };
	inline GPUMeshManager*		GetGPUMeshManager()		const { return mMeshManager.get(); };
	inline GPUMaterialManager*  GetGPUMaterialManager() const { return mMaterialManager.get(); };

	inline const GraphicsCardInformation& GetCardInfo() const { return mGraphicsCardInfo; };
	void SetCardInfo(const GraphicsCardInformation& Info) { mGraphicsCardInfo = Info; };

	inline DirectResourceHandle<GPUConstantBuffer> GetFrameConstantsBuffer() const { return mFrameConstantsBuffer; }
	inline DirectResourceHandle<GPUConstantBuffer> GetCameraConstantsBuffer() const { return mCameraConstantsBuffer; }
	inline DirectResourceHandle<GPUConstantBuffer> GetObjectConstantBuffer() const { return mObjectConstantBuffer; }

	inline CommandList* GetFrameSetupCommandList() const { return mFrameSetupCommandList; };

	inline const MainRenderTarget& GetMainRenderTarget() const { return mMainRenderTarget; };
	void ResizeMainRenderTarget(const Vector2i& Extent);

	//TODO: remove later
	inline Camera* GetActiveCamera() const { return mActiveCamera; };
	inline void SetActiveCamera(Camera* Cam) { mActiveCamera = Cam; };

private:
	Renderer();
	~Renderer();
private:
	LogicalDevice mDevice;
	GraphicsCardInformation mGraphicsCardInfo;

	OwnedPtr<SwapChain> mSwapChain;
	OwnedPtr<Sampler> mSampler;

	OwnedPtr<ShaderManager> mShaderManager;
	OwnedPtr<InputLayoutManager> mInputManager;
	OwnedPtr<RenderBufferManager> mRenderBufferManager;
	OwnedPtr<GPUTextureManager> mTextureManager;
	OwnedPtr<GPUMeshManager> mMeshManager;
	OwnedPtr<GPUMaterialManager> mMaterialManager;

	DirectResourceHandle<GPUConstantBuffer> mFrameConstantsBuffer;
	DirectResourceHandle<GPUConstantBuffer> mCameraConstantsBuffer;
	DirectResourceHandle<GPUConstantBuffer> mObjectConstantBuffer;
	
	MainRenderTarget mMainRenderTarget;

	CommandList* mFrameSetupCommandList;

	//TODO: Remove when camera manager is added
	Camera* mActiveCamera;
};

#define GET_RENDERER() SubsystemManager::Get<Renderer>()