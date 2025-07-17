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
};