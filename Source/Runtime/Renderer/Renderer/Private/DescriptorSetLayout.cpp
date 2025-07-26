#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(LogicalDevice* Device, const Vector<DescriptorBinding>& Bindings)
{
	std::vector<VkDescriptorSetLayoutBinding> vkBindings;
	for (const auto& binding : Bindings)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding.binding;
		layoutBinding.descriptorCount = binding.count;
		layoutBinding.stageFlags = binding.stageFlags;

		switch (binding.type)
		{
		case DescriptorType::UniformBuffer:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case DescriptorType::CombinedImageSampler:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		case DescriptorType::StorageImage:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			break;
		case DescriptorType::StorageBuffer:
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		default:
			_THROW_RENDERER("Trying to create unsupported descriptorset layout binding");
			break;
		}

		vkBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32>(vkBindings.size());
	layoutInfo.pBindings = vkBindings.data();

	if (vkCreateDescriptorSetLayout(*Device, &layoutInfo, nullptr, &mLayout) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create descriptorSet layout");
		return;
	}
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(*Renderer::Get()->GetDevice(), mLayout, nullptr);
}
