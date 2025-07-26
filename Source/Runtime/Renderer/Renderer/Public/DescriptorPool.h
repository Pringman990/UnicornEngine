#pragma once
#include <Core.h>

#include "DescriptorBinding.h"

class DescriptorPool
{
public:
	DescriptorPool(LogicalDevice* Device, uint32 MaxSets, const Vector<DescriptorType>& types);
	~DescriptorPool();

	VkDescriptorPool GetRaw() const { return mPool; };

private:
	VkDescriptorPool mPool;
};