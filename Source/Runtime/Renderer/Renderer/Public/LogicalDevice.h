#pragma once
#include <Core.h>

#include <Vulkan/vulkan.h>
#include "GPUQueue.h"

class Renderer;

class LogicalDevice
{
public:
	LogicalDevice();
	~LogicalDevice();

	static LogicalDevice* Create(Renderer& RendererRef, VkPhysicalDevice PhysicalDevice, GPUQueueFamilyInfo QueueFamilyInfo);

	VkDevice* GetAdressOf() noexcept { return &mDevice; };
	VkDevice GetRaw() const noexcept { return mDevice; };

	const GPUQueue& GetGraphicsQueue() const { return mGraphicsQueue; };
	const GPUQueue& GetPresentQueue() const { return mPresentQueue; };

public:
	operator VkDevice() const noexcept { return mDevice; }
private:
	VkDevice mDevice;

	GPUQueue mGraphicsQueue;
	GPUQueue mPresentQueue;
};