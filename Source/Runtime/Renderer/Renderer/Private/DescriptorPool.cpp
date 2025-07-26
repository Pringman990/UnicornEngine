#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(LogicalDevice* Device, uint32 MaxSets, const Vector<DescriptorType>& types)
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    for (DescriptorType type : types) 
    {
        VkDescriptorPoolSize size{};
        switch (type) {
        case DescriptorType::UniformBuffer:
            size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case DescriptorType::CombinedImageSampler:
            size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case DescriptorType::StorageImage:
            size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            break;
        case DescriptorType::StorageBuffer:
            size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        default:
            _THROW_RENDERER("Trying to create unsupported descriptorPool");
            break;
        }
        size.descriptorCount = MaxSets;
        poolSizes.push_back(size);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = MaxSets;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    if (vkCreateDescriptorPool(*Device, &poolInfo, nullptr, &mPool) != VK_SUCCESS) 
    {
        _THROW_RENDERER("Failed to create descriptorPool");
        return;
    }
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(*Renderer::Get()->GetDevice(), mPool, nullptr);
}
