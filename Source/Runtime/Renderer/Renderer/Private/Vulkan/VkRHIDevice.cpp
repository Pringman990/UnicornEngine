#include "Vulkan/VkRHIDevice.h"

#include "Vulkan/VkRHIRenderer.h"

VkRHIDevice::VkRHIDevice(VkRHIRenderer& VkRenderer)
	:
	mDevice(VK_NULL_HANDLE),
	mRenderer(VkRenderer)
{
}

VkRHIDevice::~VkRHIDevice()
{
	vkDestroyDevice(mDevice, nullptr);
}

bool VkRHIDevice::Create()
{
	QueueFamilyIndices indices = mRenderer.GetQueueFamilies();

	Vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	Set<uint32> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	
	float queuePriority = 1.0f;
	for (uint32 queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	auto requiredExtensions = mRenderer.GetRequiredDeviceExtensions();

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(mRenderer.GetPhysicalDevice(), &createInfo, nullptr, &mDevice);
	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create Vulkan logical device: {}", string_VkResult(result));
		return false;
	}

	vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
	
	return true;
}

void* VkRHIDevice::GetRawDevice()
{
	return nullptr;
}
