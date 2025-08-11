#pragma once
#include <Core.h>

class DescriptorLayoutBuilder
{
public:
	DescriptorLayoutBuilder() = default;
	~DescriptorLayoutBuilder() = default;

	DescriptorLayoutBuilder& AddBinding(uint32 Binding, VkDescriptorType Type, VkShaderStageFlags StageFlags);

	VkDescriptorSetLayout Build(LogicalDevice* Device);

private:
	Vector<VkDescriptorSetLayoutBinding> mBindings;
};