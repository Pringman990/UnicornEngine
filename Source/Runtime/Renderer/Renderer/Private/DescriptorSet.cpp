#include "DescriptorSet.h"

#include "Sampler.h"

DescriptorSet::DescriptorSet(LogicalDevice* Device, DescriptorPool& Pool, DescriptorSetLayout& Layout)
{
    VkDescriptorSetLayout vkLayout = Layout.GetRaw();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = Pool.GetRaw();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkLayout;

    if (vkAllocateDescriptorSets(*Device, &allocInfo, &mSet) != VK_SUCCESS) 
    {
        _THROW_RENDERER("Failed to allocate descriptor set!");
    }
}

DescriptorSet::~DescriptorSet()
{
}

void DescriptorSet::UpdateBuffer(uint32_t Binding, VkBuffer Buffer, VkDeviceSize Size)
{
}

void DescriptorSet::UpdateImage(uint32_t Binding, VkImageView ImageView, Sampler* Sampler)
{
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Or GENERAL for storage
    imageInfo.imageView = ImageView;
    imageInfo.sampler = *Sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = mSet;
    write.dstBinding = Binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(*Renderer::Get()->GetDevice(), 1, &write, 0, nullptr);
}
