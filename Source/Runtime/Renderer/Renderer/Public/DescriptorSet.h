#pragma once
#include <Core.h>

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

class Sampler;

class DescriptorSet
{
public:
    DescriptorSet(LogicalDevice* Device, DescriptorPool& Pool, DescriptorSetLayout& Layout);
    ~DescriptorSet();

    void UpdateBuffer(uint32_t Binding, VkBuffer Buffer, VkDeviceSize Size);
    void UpdateImage(uint32_t Binding, VkImageView ImageView, Sampler* Sampler);

    VkDescriptorSet GetRaw() const { return mSet; }

private:
    VkDescriptorSet mSet;
};