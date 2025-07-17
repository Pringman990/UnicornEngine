#pragma once
#include <Core.h>
#include "CommandBuffer.h"

class CommandPool
{
public:
	CommandPool();
	~CommandPool();

	/**
	* @param Transient Hint for short-lived command buffers.
	* @param AllowReset Allow individual command buffer reset
	*/
	static CommandPool* CreatePool(LogicalDevice* Device, uint32 QueueFamily, bool Transient, bool AllowReset);

	CommandBuffer* AllocateCommandBuffer(VkCommandBufferLevel Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	Vector<CommandBuffer*> AllocateCommandBuffers(uint32 Count, VkCommandBufferLevel Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	void Reset(VkCommandPoolResetFlags Flags = 0);
	void FreeCommandBuffer(CommandBuffer* Buffer);
	void FreeCommandBuffers(Vector<CommandBuffer*>& Buffers);

public:
	operator VkCommandPool() const noexcept { return mPool; }

private:
	LogicalDevice* mDevice;
	VkCommandPool mPool;
};