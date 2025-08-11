#include "DescriptorWriter.h"

DescriptorWriter::DescriptorWriter(VkDescriptorSetLayout Layout, DescriptorPool* DescPool)
	:
	mLayout(Layout),
	mPool(DescPool)
{
}

DescriptorWriter::~DescriptorWriter()
{
}

DescriptorWriter& DescriptorWriter::WriteBuffer(uint32 Binding, const VkDescriptorBufferInfo& BufferInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstBinding = Binding;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.pBufferInfo = &BufferInfo;

    mWrites.push_back(descriptorWrite);
	return *this;
}

DescriptorWriter& DescriptorWriter::WriteImage(uint32 Binding, const VkDescriptorImageInfo& ImageInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstBinding = Binding;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.pImageInfo = &ImageInfo;

    mWrites.push_back(descriptorWrite);
    return *this;
}

DescriptorWriter& DescriptorWriter::WriteStorageImage(uint32 Binding, const VkDescriptorImageInfo& ImageInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstBinding = Binding;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;  // Correct type for storage image
    descriptorWrite.pImageInfo = &ImageInfo;

    mWrites.push_back(descriptorWrite);
    return *this;
}

DescriptorSet* DescriptorWriter::Build(LogicalDevice* Device)
{
    DescriptorSet* set = new DescriptorSet();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *mPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &mLayout;

    if (vkAllocateDescriptorSets(*Device, &allocInfo, &set->mSet) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to create descriptor set!");
        return nullptr;
    }

    for (auto& write : mWrites)
    {
        write.dstSet = set->mSet;
    }

    vkUpdateDescriptorSets(*Device, static_cast<uint32_t>(mWrites.size()), mWrites.data(), 0, nullptr);

	return set;
}
