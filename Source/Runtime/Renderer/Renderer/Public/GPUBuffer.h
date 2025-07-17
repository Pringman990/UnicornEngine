#pragma once

struct GPUBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;

	void Destory(LogicalDevice* Device)
	{
		vkDestroyBuffer(*Device, buffer, nullptr);
		vkFreeMemory(*Device, memory, nullptr);
		size = {};
	}
};