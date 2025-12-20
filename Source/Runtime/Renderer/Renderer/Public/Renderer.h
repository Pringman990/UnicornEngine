#pragma once
#include "RendererDefines.h"
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
	RENDERER_API static Renderer* Instance();

	RENDERER_API bool Init();

	RENDERER_API void SubmitMesh(AssetRef<Mesh> Mesh, const Transform& ObjectTransfrom);
	RENDERER_API void SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom, Vector<Material*> OverrideMaterials);

	RENDERER_API void HandleResizeEvent(int32 Width, int32 Height);

	RENDERER_API inline const LogicalDevice& GetLogicalDevice() const { return mDevice; };
	RENDERER_API inline SwapChain* GetSwapChain() const { return mSwapChain.get(); };
	
	RENDERER_API inline ShaderManager* GetShaderManager() const { return mShaderManager.get(); };
	RENDERER_API inline InputLayoutManager* GetInputManager() const { return mInputManager.get(); };
	RENDERER_API inline RenderBufferManager* GetBufferManager() const { return mRenderBufferManager.get(); };
	
	RENDERER_API inline GPUTextureManager*	GetGPUTextureManager()	const { return mTextureManager.get(); };
	RENDERER_API inline GPUMeshManager*		GetGPUMeshManager()		const { return mMeshManager.get(); };
	RENDERER_API inline GPUMaterialManager*  GetGPUMaterialManager() const { return mMaterialManager.get(); };

	RENDERER_API inline const GraphicsCardInformation& GetCardInfo() const { return mGraphicsCardInfo; };
	RENDERER_API void SetCardInfo(const GraphicsCardInformation& Info) { mGraphicsCardInfo = Info; };

	RENDERER_API inline DirectResourceHandle<GPUConstantBuffer> GetFrameConstantsBuffer() const { return mFrameConstantsBuffer; }
	RENDERER_API inline DirectResourceHandle<GPUConstantBuffer> GetCameraConstantsBuffer() const { return mCameraConstantsBuffer; }
	RENDERER_API inline DirectResourceHandle<GPUConstantBuffer> GetObjectConstantBuffer() const { return mObjectConstantBuffer; }

	RENDERER_API inline CommandList* GetFrameSetupCommandList() const { return mFrameSetupCommandList; };

	RENDERER_API inline const MainRenderTarget& GetMainRenderTarget() const { return mMainRenderTarget; };
	RENDERER_API void ResizeMainRenderTarget(const Vector2i& Extent);

	//TODO: remove later
	RENDERER_API inline Camera* GetActiveCamera() const { return mActiveCamera; };
	RENDERER_API inline void SetActiveCamera(Camera* Cam) { mActiveCamera = Cam; };

private:
	Renderer();
	~Renderer();
private:
	static Renderer* sInstance;

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