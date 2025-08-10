#pragma once
#include <Core.h>

class DescriptorSet
{
	friend class DescriptorWriter;
public:
	DescriptorSet();
	~DescriptorSet();

	const VkDescriptorSet& GetRaw() { return mSet; };
	const VkDescriptorSet* GetAdressOf() { return &mSet; };

private:
	VkDescriptorSet mSet;
};