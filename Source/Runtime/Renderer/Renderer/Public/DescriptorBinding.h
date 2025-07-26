#pragma once
#include <Core.h>

enum class DescriptorType
{
	UniformBuffer,
	CombinedImageSampler,
	StorageImage,
	StorageBuffer
};

struct DescriptorBinding
{
	DescriptorBinding(uint32 Slot, DescriptorType Type, VkShaderStageFlags Stages, uint32 Count = 1)
		: binding(Slot), type(Type), count(Count), stageFlags(Stages)
	{

	};

	uint32 binding;
	DescriptorType type;
	uint32 count = 1;
	VkShaderStageFlags stageFlags;
};