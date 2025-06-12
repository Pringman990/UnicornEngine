#pragma once
#include <Core.h>

#include <Vulkan/vulkan.h>

#include "Internal/IRHIDevice.h"

class VkRHIRenderer;

class VkRHIDevice : public IRHIDevice
{
public:
	VkRHIDevice(VkRHIRenderer& VkRenderer);
	~VkRHIDevice() override;

	virtual bool Create() override;
	virtual void* GetRawDevice() override;

	__forceinline VkDevice GetDevice() { return mDevice; };

	VkDevice operator()() { return mDevice; }
	operator VkDevice() { return operator()(); }

	VkQueue GetGraphicsQueue() { return mGraphicsQueue; };
	VkQueue GetPresentQueue() { return mPresentQueue; };

private:
	VkDevice mDevice;
	VkRHIRenderer& mRenderer;
	VkQueue mGraphicsQueue;
	VkQueue mPresentQueue;
};