#include "DescriptorLayoutBuilder.h"

DescriptorLayoutBuilder& DescriptorLayoutBuilder::AddBinding(uint32 Binding, VkDescriptorType Type, VkShaderStageFlags StageFlags)
{
	VkDescriptorSetLayoutBinding bind{};
	bind.binding = Binding;
	bind.descriptorType = Type;
	bind.descriptorCount = 1;
	bind.stageFlags = StageFlags;
	bind.pImmutableSamplers = nullptr;

	mBindings.push_back(bind);
	return *this;
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(LogicalDevice* Device)
{
	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32>(mBindings.size());
	createInfo.pBindings = mBindings.data();

	VkDescriptorSetLayout layout;
	if (vkCreateDescriptorSetLayout(*Device, &createInfo, nullptr, &layout) != VK_SUCCESS)
	{
		_LOG_RENDERER_ERROR("Failed to build VkDescriptorSetLayout");
		return nullptr;
	}

	return layout;
}
