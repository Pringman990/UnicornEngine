#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice()
{
}

PhysicalDevice::~PhysicalDevice()
{
	mPhysicalDevice = VK_NULL_HANDLE;
}

PhysicalDevice* PhysicalDevice::Create(VkInstance Instance, Surface* Surface)
{
	PhysicalDevice* physicalDevice = new PhysicalDevice();

	uint32 deviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);
	if (deviceCount == 0 || result != VK_SUCCESS)
	{
		_ASSERT_RENDERER(false, "No supported physical device: {}", string_VkResult(result));
	}

	Vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	struct SuitablePhysicalDevice
	{
		VkPhysicalDevice physicalDevice;
		GPUQueueFamilyInfo familyInfo;
	};

	MultiMap<int32, SuitablePhysicalDevice> candidates;

	for (const auto& device : devices)
	{
		GPUQueueFamilyInfo familyInfo;
		if (IsPhysicalDeviceSuitable(device, Surface, familyInfo))
		{
			int32 score = RatePhysicalDeviceSuitability(device);
			
			SuitablePhysicalDevice suitableDevice;
			suitableDevice.physicalDevice = device;
			suitableDevice.familyInfo = familyInfo;
			
			candidates.insert(std::make_pair(score, suitableDevice));
		}
	}

	if (candidates.rbegin()->first > 0)
	{
		physicalDevice->mPhysicalDevice = candidates.rbegin()->second.physicalDevice;
		physicalDevice->mQueueFamilyInfo = candidates.rbegin()->second.familyInfo;
	}
	else
	{
		_ASSERT_RENDERER(false, "No supported physical device even when device(s) exists");
	}

	physicalDevice->FillGraphicsCardInfo();

	return physicalDevice;
}

LogicalDevice* PhysicalDevice::CreateLogicalDevice()
{
	return LogicalDevice::Create(Renderer::GetRef(), mPhysicalDevice, mQueueFamilyInfo);
}

SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(Surface* Surface) const
{
	return QuerySwapChainSupport(mPhysicalDevice, Surface);
}

GPUQueueFamilyInfo PhysicalDevice::FindQueueFamilies(Surface* Surface) const
{
	return FindQueueFamilies(mPhysicalDevice, Surface);
}

uint32 PhysicalDevice::FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Flags) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & Flags) == Flags)
		{
			return i;
		}
	}

	_THROW_RENDERER("No available memory type");

	return uint32();
}

SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice PhysicalDevice, Surface* Surface)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, *Surface, &details.capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, *Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, *Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, *Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, *Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool PhysicalDevice::IsPhysicalDeviceSuitable(VkPhysicalDevice PhysicalDevice, Surface* Surface, GPUQueueFamilyInfo& QueueFamilyInfo)
{
	GPUQueueFamilyInfo indices = FindQueueFamilies(PhysicalDevice, Surface);

	bool extensionsSupported = CheckPhysicalDeviceExtensionSupport(PhysicalDevice);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice, Surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	if (indices.isComplete() && extensionsSupported && swapChainAdequate)
	{
		QueueFamilyInfo = indices;
		return true;
	}

	return false;
}

int32 PhysicalDevice::RatePhysicalDeviceSuitability(VkPhysicalDevice PhysicalDevice)
{
	//Add requirements for choosing a physical device

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &deviceFeatures);

	int32 score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader)
	{
		return 0;
	}

	return score;
}

bool PhysicalDevice::CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice PhysicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

	const Vector<const char*> extensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	Set<String> requiredExtensions(extensions.begin(), extensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

GPUQueueFamilyInfo PhysicalDevice::FindQueueFamilies(VkPhysicalDevice PhysicalDevice, Surface* Surface)
{
	GPUQueueFamilyInfo indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	for (int32 i = 0; i < queueFamilies.size(); i++)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, *Surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		if (indices.isComplete())
			break;
	}

	return indices;
}

void PhysicalDevice::FillGraphicsCardInfo()
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &deviceFeatures);

	mCardInfo.name = deviceProperties.deviceName;
	mCardInfo.vendorID = deviceProperties.vendorID;
}
