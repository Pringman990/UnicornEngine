#pragma once
#include <Core.h>

class DescriptorPoolBuilder
{
public:
	DescriptorPoolBuilder() = default;
	~DescriptorPoolBuilder() = default;

	DescriptorPoolBuilder& AddPoolSize(VkDescriptorType Type, uint32_t Count);

	DescriptorPool* Build(LogicalDevice* Device, uint32_t MaxSets, VkDescriptorPoolCreateFlags Flags = 0);

private:
	Vector<VkDescriptorPoolSize> mPoolSizes;
};