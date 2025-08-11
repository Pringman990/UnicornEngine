#pragma once
#include <Core.h>

class DescriptorPool
{
	friend class DescriptorPoolBuilder;
public:
	DescriptorPool();
	~DescriptorPool();

public:
	operator VkDescriptorPool() const noexcept { return mPool; }
private:
	VkDescriptorPool mPool;
};