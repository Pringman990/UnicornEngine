#pragma once
#include <Core.h>

#include "LogicalDevice.h"

struct GPUTexture
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	
	VkFormat format;
	VkExtent3D extent;
	VkImageLayout currentImageLayout;

	static void DestroyGPUTexture(GPUTexture* Texture, LogicalDevice* Device)
	{
		vkDestroyImage(*Device, Texture->image, nullptr);
		vkDestroyImageView(*Device, Texture->imageView, nullptr);
		vkFreeMemory(*Device, Texture->memory, nullptr);
	}

	static void DestroyGPUTextureImageViewOnly(GPUTexture* Texture, LogicalDevice* Device)
	{
		vkDestroyImageView(*Device, Texture->imageView, nullptr);
	}
};