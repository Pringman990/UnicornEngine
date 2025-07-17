#pragma once
#include <Core.h>

#include "LogicalDevice.h"

class CommandPool;

class CommandBuffer
{
public:
	CommandBuffer();
	CommandBuffer(LogicalDevice* Device, CommandPool* Pool, VkCommandBuffer Buffer);

	void Begin(VkCommandBufferUsageFlags Flags = 0);
	void End();
	void EndWithSubmit();
	void Reset(VkCommandBufferResetFlags Flags = 0);

	VkCommandBuffer* GetAdressOf() noexcept { return &mBuffer; };
	VkCommandBuffer GetRaw() const noexcept { return mBuffer; };

public:
	operator VkCommandBuffer() const noexcept { return mBuffer; }

private:
	VkCommandBuffer mBuffer;

	LogicalDevice* mDevice;
	CommandPool* mPool;
};