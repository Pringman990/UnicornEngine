#pragma once
#include <Core.h>
#include "LogicalDevice.h"
#include "GPUQueue.h"
#include "Surface.h"

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	Vector<VkSurfaceFormatKHR> formats;
	Vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice
{
public:
	PhysicalDevice();
	~PhysicalDevice();

	static PhysicalDevice* Create(VkInstance Instance, Surface* Surface);

	LogicalDevice* CreateLogicalDevice();

	SwapChainSupportDetails QuerySwapChainSupport(Surface* Surface) const;
	GPUQueueFamilyInfo FindQueueFamilies(Surface* Surface) const;
	uint32 FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Flags) const;
public:
	operator VkPhysicalDevice() const noexcept { return mPhysicalDevice; }
private:
	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice PhysicalDevice, Surface* Surface);
	static bool IsPhysicalDeviceSuitable(VkPhysicalDevice PhysicalDevice, Surface* Surface, GPUQueueFamilyInfo& QueueFamilyInfo);
	static int32 RatePhysicalDeviceSuitability(VkPhysicalDevice PhysicalDevice);
	static bool CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice PhysicalDevice);
	static GPUQueueFamilyInfo FindQueueFamilies(VkPhysicalDevice PhysicalDevice, Surface* Surface);

	void FillGraphicsCardInfo();
private:
	VkPhysicalDevice mPhysicalDevice;
	GPUQueueFamilyInfo mQueueFamilyInfo;

	GraphicsCardInfo mCardInfo;
};