#pragma once
#include <Core.h>

struct GPUQueue
{
	VkQueue queue;
	uint32 index;
};

struct GPUQueueFamilyInfo
{
	std::optional<uint32> graphicsFamily;
	std::optional<uint32> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};