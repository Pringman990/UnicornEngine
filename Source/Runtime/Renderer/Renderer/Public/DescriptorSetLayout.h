#pragma once
#include <Core.h>

#include "DescriptorBinding.h"

class DescriptorSetLayout
{
public:
	DescriptorSetLayout(LogicalDevice* Device, const Vector<DescriptorBinding>& Bindings);
	~DescriptorSetLayout();

	VkDescriptorSetLayout GetRaw() const { return mLayout; }

private:
	VkDescriptorSetLayout mLayout;
};