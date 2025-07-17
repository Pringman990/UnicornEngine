#include "CommandPool.h"

CommandPool::CommandPool()
{
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(*mDevice, mPool, nullptr);
	mPool = VK_NULL_HANDLE;
	mDevice = nullptr;
}

CommandPool* CommandPool::CreatePool(LogicalDevice* Device, uint32 QueueFamily, bool Transient, bool AllowReset)
{
	CommandPool* pool = new CommandPool();
	pool->mDevice = Device;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = QueueFamily;
	poolInfo.flags = 0;

	if (AllowReset)
		poolInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (Transient)
		poolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	if (vkCreateCommandPool(*Device, &poolInfo, nullptr, &pool->mPool) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create command pool");
		return nullptr;
	}

	return pool;
}

CommandBuffer* CommandPool::AllocateCommandBuffer(VkCommandBufferLevel Level)
{	
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mPool;
	allocInfo.level = Level;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(*mDevice, &allocInfo, &commandBuffer) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create command buffer");
		return nullptr;
	}

	CommandBuffer* buffer = new CommandBuffer(mDevice, this, commandBuffer);
	return buffer;
}

Vector<CommandBuffer*> CommandPool::AllocateCommandBuffers(uint32 Count, VkCommandBufferLevel Level)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mPool;
	allocInfo.level = Level;
	allocInfo.commandBufferCount = Count;

	Vector<VkCommandBuffer> commandBuffers(Count);
	if (vkAllocateCommandBuffers(*mDevice, &allocInfo, &commandBuffers[0]) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create command buffer");
		return Vector<CommandBuffer*>();
	}

	Vector<CommandBuffer*> buffers(Count);
	for (uint32 i = 0; i < Count; i++)
	{
		buffers.push_back(new CommandBuffer(mDevice, this, commandBuffers[i]));
	}

	return buffers;
}

void CommandPool::Reset(VkCommandPoolResetFlags Flags)
{
	vkResetCommandPool(*mDevice, mPool, Flags);
}

void CommandPool::FreeCommandBuffer(CommandBuffer* Buffer)
{
	vkFreeCommandBuffers(*mDevice, mPool, 1, Buffer->GetAdressOf());
}

void CommandPool::FreeCommandBuffers(Vector<CommandBuffer*>& Buffers)
{
	Vector<VkCommandBuffer> rawBuffers(Buffers.size());
	std::transform(Buffers.begin(), Buffers.end(), rawBuffers.begin(), [](const CommandBuffer* b)
		{
			return b->GetRaw();
		});

	vkFreeCommandBuffers(*mDevice, mPool, static_cast<uint32>(Buffers.size()), rawBuffers.data());
}
