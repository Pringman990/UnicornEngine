#include "DescriptorPoolBuilder.h"

DescriptorPoolBuilder& DescriptorPoolBuilder::AddPoolSize(VkDescriptorType Type, uint32_t Count)
{
	mPoolSizes.push_back({Type, Count});
	return *this;
}

DescriptorPool* DescriptorPoolBuilder::Build(LogicalDevice* Device, uint32_t MaxSets, VkDescriptorPoolCreateFlags Flags)
{
	DescriptorPool* pool = new DescriptorPool();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32>(mPoolSizes.size());
    poolInfo.pPoolSizes = mPoolSizes.data();
    poolInfo.maxSets = MaxSets;  // Number of descriptor sets to allocate
    poolInfo.flags = Flags;

    if (vkCreateDescriptorPool(*Device, &poolInfo, nullptr, &pool->mPool) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to create descriptor pool!");
        return nullptr;
    }

	return pool;
}
