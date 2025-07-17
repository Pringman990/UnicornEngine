#include "GPUFence.h"

GPUFence::GPUFence()
	:
	mFence(VK_NULL_HANDLE),
	mDevice(nullptr)
{
}

GPUFence::~GPUFence()
{
	vkDestroyFence(*mDevice, mFence, nullptr);
	mFence = VK_NULL_HANDLE;
	mDevice = nullptr;
}

GPUFence* GPUFence::Create(LogicalDevice* Device, bool Signaled)
{
	GPUFence* fence = new GPUFence();
	fence->mDevice = Device;

	VkFenceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.flags = Signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if(vkCreateFence(*Renderer::Get()->GetDevice(), &createInfo, nullptr, &fence->mFence) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create fence");
		return nullptr;
	}

	return fence;
}

void GPUFence::Wait(uint64 Timeout) const 
{
	vkWaitForFences(*mDevice, 1, &mFence, VK_TRUE, Timeout);
}

void GPUFence::Reset() 
{
	vkResetFences(*mDevice, 1, &mFence);
}

bool GPUFence::IsSignaled() const
{
	return vkGetFenceStatus(*mDevice, mFence) == VK_SUCCESS;
}
