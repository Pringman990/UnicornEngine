#pragma once
#include <Core.h>
#include <Vulkan/vulkan.h>
#include <Vulkan/vk_enum_string_helper.h>

#include "Internal/IRHIRenderer.h"
#include "VkRHIDevice.h"

#define MAX_FRAMES_IN_FLIGHT 2

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
private:
	bool CreateInstance();
	bool CreateSurface();
	bool CreatePhysicalDevice();
	bool CreateSwapChain();
	bool CreateGraphicsPipeline();
	bool CreateRenderPass();
	bool CreateFrameBuffers();
	bool CreateCommandPool();
	bool CreateVertexBuffer();
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
		{{0.0f, -0.5f}, {1.0f,0.0f,0.0f,1.0f}},
		{{0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}},
		{{-0.5f, 0.5f}, {0.0f,0.0f,1.0f,1.0f}}
	};
	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;
};