#include "SwapChain.h"

#include "PhysicalDevice.h"
#include "LogicalDevice.h"

#include "Factories/TextureFactory.h"

SwapChain::SwapChain()
	:
	mSwapChain(VK_NULL_HANDLE),
	mMinImageCountSupported(0),
	mImageCount(0),
	mImageFormat(VK_FORMAT_UNDEFINED),
	mExtent({0,0})
{
}

SwapChain::~SwapChain()
{
	Destroy();
}

SwapChain* SwapChain::Create(PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface)
{
	SwapChain* swapChain = new SwapChain();

	Create(swapChain, PhysicalDevice, Logical, Surface);

    return swapChain;
}

void SwapChain::ReCreate(PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface)
{
	vkDeviceWaitIdle(*Logical);

	Destroy();
	Create(this, PhysicalDevice, Logical, Surface);
}

void SwapChain::Destroy() 
{
	LogicalDevice* device = Renderer::Get()->GetDevice();

	for (uint32 i = 0; i < mRenderTargets.size(); i++)
	{
		Renderer::Get()->GetGPUResourceManager().FreeResource<GPUTexture>(mRenderTargets[i], &GPUTexture::DestroyGPUTextureImageViewOnly, device);
	}

	vkDestroySwapchainKHR(*device, mSwapChain, nullptr);
	mSwapChain = VK_NULL_HANDLE;

	mRenderTargets.clear();

	mMinImageCountSupported = 0;
	mImageCount = 0;
	mImageFormat = VK_FORMAT_UNDEFINED;
	mExtent = {};
}

void SwapChain::Create(SwapChain* SwapChainPtr, PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface)
{
	SwapChainSupportDetails swapChainSupport = PhysicalDevice->QuerySwapChainSupport(Surface);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentFormat(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	SwapChainPtr->mMinImageCountSupported = imageCount;
	SwapChainPtr->mImageFormat = surfaceFormat.format;
	SwapChainPtr->mExtent = extent;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	GPUQueueFamilyInfo indices = PhysicalDevice->FindQueueFamilies(Surface);
	uint32_t queueFamilyIndices[] =
	{
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(*Logical, &createInfo, nullptr, &SwapChainPtr->mSwapChain);
	_ASSERT_RENDERER(result == VK_SUCCESS, "Failed to create SwapChain");

	Vector<VkImage> vkSwapChainImages;
	vkGetSwapchainImagesKHR(*Logical, SwapChainPtr->mSwapChain, &imageCount, nullptr);
	vkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*Logical, SwapChainPtr->mSwapChain, &imageCount, vkSwapChainImages.data());

	SwapChainPtr->mImageCount = imageCount;

	SwapChainPtr->mRenderTargets.resize(imageCount);
	for (uint32 i = 0; i < imageCount; i++)
	{
		SwapChainPtr->mRenderTargets[i] = TextureFactory::CreateTextureRenderTargetSC(*Logical, vkSwapChainImages[i], surfaceFormat.format, extent);
		_ASSERT_RENDERER(SwapChainPtr->mRenderTargets[i], "Failed to create swapchain rendertarget");
	}
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& AvailableFormats)
{
	for (const auto& availableFormat : AvailableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			)
		{
			return availableFormat;
		}
	}

	return VkSurfaceFormatKHR();
}

VkPresentModeKHR SwapChain::ChooseSwapPresentFormat(const Vector<VkPresentModeKHR>& AvailablePresentModes)
{
	for (const auto& availablePresentMode : AvailablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

#undef max
VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilites)
{
	if (Capabilites.currentExtent.width != std::numeric_limits<uint32>::max())
	{
		return Capabilites.currentExtent;
	}
	else
	{
		GenericApplication* app = Application::Get()->GetApplication();
		auto& windowInfo = app->GetWindowInfo();

		VkExtent2D actualExtent =
		{
			std::clamp(windowInfo.viewportWidth, Capabilites.minImageExtent.width, Capabilites.maxImageExtent.width),
			std::clamp(windowInfo.viewportHeight, Capabilites.minImageExtent.height, Capabilites.maxImageExtent.height)
		};

		return actualExtent;
	}
}
