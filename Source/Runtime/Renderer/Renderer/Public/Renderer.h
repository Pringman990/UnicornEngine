#pragma once
#include <Core.h>
#include "EngineSubsystem.h"
#include <Vulkan/vulkan.h>
#include <Vulkan/vk_enum_string_helper.h>

#include <ResourceManagment/ResourcePool.h>

#include "GraphicsCardInfo.h"

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "SwapChain.h"
#include "CommandPool.h"
#include "GPUFence.h"
#include "Semaphore.h"

#include "Camera.h"
#include "Vertex.h"

#include "GPUResourceManager.h"
#include "GenericGPUBuffer.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"

#define VULKAN_ALIGN alignas(16)
#define VK_ENABLE_VALIDATION_LAYER_INFO_LOGGING false

//TEMP
struct ObjectFrameDataUBO
{
	VULKAN_ALIGN Matrix model;
};

struct FrameSync
{
	GPUFence* inFlightFence;
	Semaphore* imageAvailable;
	Semaphore* renderFinished;
	CommandPool* commandPool;
	CommandBuffer* commandBuffer;
	uint32 imageIndex;

	GenericGPUBuffer* cameraUBOBuffer;
	GenericGPUBuffer* objectFrameDataUBOBuffer;
	DescriptorSet* descSet;

	Vector<SharedPtr<GenericGPUBuffer>> perFrameBuffers;

	void Destroy()
	{
		delete inFlightFence;
		inFlightFence = nullptr;

		delete imageAvailable;
		imageAvailable = nullptr;

		delete renderFinished;
		renderFinished = nullptr;

		delete commandPool;
		commandPool = nullptr;
		
		delete commandBuffer;
		commandBuffer = nullptr;
	}
};

struct DrawCallsInfo
{
	uint32 editorDrawCalls = 0;
	uint32 gameDrawCalls = 0;

	uint32 GetTotal() const
	{
		return editorDrawCalls + gameDrawCalls;
	}

	void Reset()
	{
		editorDrawCalls = 0;
		gameDrawCalls = 0;
	}
};

struct RenderScopeInfo;

class Renderer final : public EngineSubsystem<Renderer>
{
public:
	Renderer();
	~Renderer();

	bool Init();
	void BeginFrame();
	void EndFrame();

	VkInstance GetInstance() { return mInstance; };
	PhysicalDevice* GetPhysicalDevice() const { return mPhysicalDevice; };
	LogicalDevice* GetDevice() const { return mDevice; };
	SwapChain* GetSwapChain() const { return mSwapChain; };

	uint32 GetGameDrawCalls() const { return mDrawCalls.gameDrawCalls; };
	uint32 GetEditorDrawCalls() const { return mDrawCalls.editorDrawCalls; };
	uint32 GetTotalDrawCalls() const { return mDrawCalls.GetTotal(); };

	void AddGameDrawCalls() { mDrawCalls.gameDrawCalls++; };
	void AddToGameDrawCalls(uint32 Calls) { mDrawCalls.gameDrawCalls += Calls; };
	void AddEditorDrawCalls() { mDrawCalls.editorDrawCalls++; };
	void AddToEditorDrawCalls(uint32 Calls) { mDrawCalls.editorDrawCalls += Calls; };

	const GraphicsCardInfo& GetGraphicsCardInfo() const { return mGraphicsCardInfo; };

	const GPUResourceHandle<GPUTexture> GetCurrentSwapChainTexture() { return mSwapChain->GetRenderTarget(mCurrentFrameIndex); };

	CommandBuffer* GetCurrentFrameSyncCommandBuffer() const { return mFrameSyncs[mCurrentFrameIndex].commandBuffer; };
	CommandPool* GetCurrentFrameSyncCommandPool() const { return mFrameSyncs[mCurrentFrameIndex].commandPool; };
	void PushToCurrentFrameSyncGPUBuffer(SharedPtr<GenericGPUBuffer> Buffer) { mFrameSyncs[mCurrentFrameIndex].perFrameBuffers.push_back(Buffer); };

	//const GPUResourceHandle<TextureRenderTarget> GetOffscreenTexture() const { return mOffscreenQuadTexture; };

	GPUResourceManager& GetGPUResourceManager() { return mGPUResourceManager; };

	void SetActiveCamera(Camera* ActiveCamera) { mActiveCamera = ActiveCamera; };

	//Temp
	void RenderMesh(const GPUResourceHandle<struct GPUMesh>& MeshHandle, Transform& MeshTransform);

private:
	bool CreateInstance();
	bool CheckValidationLayerSupport();
	void RegisterAllGPUResourcePools();
	void UnRegisterAllGPUResourcePools();

	const std::vector<const char*>& GetValidationLayers();
private:
	VkInstance mInstance;
	
	Surface* mSurface;
	PhysicalDevice* mPhysicalDevice;
	LogicalDevice* mDevice;
	SwapChain* mSwapChain;
	
	GraphicsCardInfo mGraphicsCardInfo;
	DrawCallsInfo mDrawCalls;
	bool mVsync;
	//TODO: move to rendergraph or other, where the submit happens
	Camera* mActiveCamera;

	uint32 mCurrentFrameIndex;
	Vector<FrameSync> mFrameSyncs;

	VkDebugUtilsMessengerEXT mDebugMessenger;
	
	GPUResourceManager mGPUResourceManager;

	//TEMP
	class Pipeline* mPipeline;
	class VertexShader* mVertexShader;
	class FragmentShader* mFragmentShader;

	class Pipeline* mVoxelPipeline;
	class ComputeShader* mVoxelCS;
	GPUResourceHandle<GPUTexture> mOffscreenQuadTexture;
	struct GPUMesh* mMesh;
	VkRect2D scissor;
	VkViewport viewport;

	DescriptorPool* mDescPool;
	VkDescriptorSetLayout mSetLayout;

	Transform mModelTransform;
};