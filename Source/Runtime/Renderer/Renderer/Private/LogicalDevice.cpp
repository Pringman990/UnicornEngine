#include "LogicalDevice.h"

#include "Renderer.h"

LogicalDevice::LogicalDevice()
	:
	mDevice(VK_NULL_HANDLE),
	mGraphicsQueue({}),
	mPresentQueue({})
{
}

LogicalDevice::~LogicalDevice()
{
	vkDestroyDevice(mDevice, nullptr);
	mDevice = VK_NULL_HANDLE;
}

LogicalDevice* LogicalDevice::Create(Renderer& RendererRef, VkPhysicalDevice PhysicalDevice, GPUQueueFamilyInfo QueueFamilyInfo)
{
	LogicalDevice* device = new LogicalDevice();

	Vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	Set<uint32> uniqueQueueFamilies = { QueueFamilyInfo.graphicsFamily.value(), QueueFamilyInfo.presentFamily.value()};
	
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
	
	const Vector<const char*> requiredExtensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = &dynamicRenderingFeatures;

	VkResult result = vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &device->mDevice);
	if (result != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create Vulkan logical device: {}", string_VkResult(result));
	}

	vkGetDeviceQueue(device->mDevice, QueueFamilyInfo.graphicsFamily.value(), 0, &device->mGraphicsQueue.queue);
	vkGetDeviceQueue(device->mDevice, QueueFamilyInfo.presentFamily.value(), 0, &device->mPresentQueue.queue);

	return device;
}
