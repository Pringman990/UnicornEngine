#pragma once
#include <Core.h>

struct GPUTexture
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	
	VkFormat format;
	VkExtent3D extent;
	VkImageLayout currentImageLayout;

	static void Free(GPUTexture* Resource, VkDevice Device)
	{
		vkDestroyImage(Device, Resource->image, nullptr);
		vkDestroyImageView(Device, Resource->imageView, nullptr);
		vkFreeMemory(Device, Resource->memory, nullptr);
	}

	static void FreeViewOnly(GPUTexture* Resource, VkDevice Device)
	{
		vkDestroyImageView(Device, Resource->imageView, nullptr);
	}
};