#pragma once
#include <Core.h>

class DescriptorWriter
{
public:
	DescriptorWriter(VkDescriptorSetLayout Layout, DescriptorPool* DescPool);
	~DescriptorWriter();

	DescriptorWriter& WriteBuffer(uint32 Binding, const VkDescriptorBufferInfo& BufferInfo);
	DescriptorWriter& WriteImage(uint32 Binding, const VkDescriptorImageInfo& ImageInfo);
	DescriptorWriter& WriteStorageImage(uint32 Binding, const VkDescriptorImageInfo& ImageInfo);

	DescriptorSet* Build(LogicalDevice* Device);

private:
	Vector<VkWriteDescriptorSet> mWrites;
	VkDescriptorSetLayout mLayout;
	DescriptorPool* mPool;
};