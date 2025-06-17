#pragma once
#include <Core.h>
#include <Vulkan/vulkan.h>
#include <Vulkan/vk_enum_string_helper.h>

#include "Internal/IRHIRenderer.h"
#include "VkRHIDevice.h"
#include "VkTextureFactory.h"

#include "Camera.h"

#define MAX_FRAMES_IN_FLIGHT 2
#define VULKAN_ALIGN alignas(16)
//TEMP
struct UniformBufferObject
{
	VULKAN_ALIGN Matrix model;
	VULKAN_ALIGN Matrix worldToClip;
};

struct QueueFamilyIndices
{
	std::optional<uint32> graphicsFamily;
	std::optional<uint32> presentFamily;

	bool isComplete() 
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	Vector<VkSurfaceFormatKHR> formats;
	Vector<VkPresentModeKHR> presentModes;
};

class VkRHIRenderer final : public IRHIRenderer
{
public:
	VkRHIRenderer();
	~VkRHIRenderer() override;

	bool Init() override;
	void PreRender() override;
	void Present() override;

	/// <summary>
	/// Returns the Queue families from the currently active physical device
	/// </summary>
	/// <returns></returns>
	QueueFamilyIndices GetQueueFamilies();

	const Vector<const char*> GetRequiredDeviceExtensions() const;

	VkPhysicalDevice& GetPhysicalDevice() { return mPhysicalDevice; };
	IRHIDevice* GetDevice() const override { return mDevice.get(); };
	ITextureFactory* GetTextureFactory() const override { return mTextureFactory.get(); };
private:
	bool CreateInstance();
	bool CreateSurface();
	bool CreatePhysicalDevice();
	bool CreateSwapChain();
	bool CreateDescriptorSetLayout();
	bool CreateGraphicsPipeline();
	bool CreateRenderPass();
	bool CreateFrameBuffers();
	bool CreateCommandPool();
	bool CreateVertexBuffer(); //Temp
	bool CreateIndexBuffer(); //Temp
	bool CreateUniformBuffers(); 
	bool CreateDescriptorPool(); 
	bool CreateDescriptorSets(); 
	bool CreateCommandBuffers();
	bool CreateSyncObjects();

	void ReCreateSwapChain();
	void CleanupSwapChain();
	
	void RecordCommandBuffer(VkCommandBuffer Buffer, uint32 ImageIndex);

	//TEMP
	void CreateSwapChainImageViews();
	VkShaderModule CreateShaderModule(const ByteBuffer& ShaderCode);
	static VkVertexInputBindingDescription GetVertexBindingDescription();
	static Array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory);
	void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);

	void UpdateUniformBuffer();

	uint32 FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Flags);

	bool IsDeviceSuitable(VkPhysicalDevice Device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice Device);
	int32 RateDeviceSuitability(VkPhysicalDevice Device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentFormat(const Vector<VkPresentModeKHR>& AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilites);
private:
	OwnedPtr<VkRHIDevice> mDevice;
	OwnedPtr<VkTextureFactory> mTextureFactory;

	VkInstance mInstance;
	VkPhysicalDevice mPhysicalDevice;
	VkSurfaceKHR mSurface;
	VkSwapchainKHR mSwapChain;
	
	//TODO: this will be a rendertarget class and a texture2d class
	Vector<VkImage> mSwapChainImages;
	Vector<VkImageView> mSwapChainImageViews;

	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;

	VkRenderPass mMainPass;
	VkPipelineLayout mPipelineLayout;
	VkPipeline mGraphicsPipeline;

	Vector<VkFramebuffer> mSwapChainFramebuffers;
	VkCommandPool mCommandPool;
	VkCommandBuffer mCommandBuffer[MAX_FRAMES_IN_FLIGHT];

	VkSemaphore mImageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore mRenderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
	VkFence mInFlightFence[MAX_FRAMES_IN_FLIGHT];

	uint32 mCurrentFrameIndex;

	//TEMP
	const Vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f,0.0f,0.0f,1.0f}},
		{{0.5f, -0.5f}, {0.0f,1.0f,0.0f,1.0f}},
		{{0.5f, 0.5f}, {0.0f,0.0f,1.0f,1.0f}},
		{{-0.5f, 0.5f}, {1.0f,1.0f,1.0f,1.0f}}
	};
	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;
	
	const Vector<uint16> mindices = {
		0, 1, 2, 2, 3, 0
	};
	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;

	VkDescriptorSetLayout mDescriptorSetLayout;

	VkBuffer uniformBuffers[MAX_FRAMES_IN_FLIGHT];
	VkDeviceMemory uniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
	void* uniformBuffersMapped[MAX_FRAMES_IN_FLIGHT];
	
	VkDescriptorPool descriptorPool;
	Vector<VkDescriptorSet> descriptorSets;

	Camera mCamera;
	Matrix modelMatrix;
};