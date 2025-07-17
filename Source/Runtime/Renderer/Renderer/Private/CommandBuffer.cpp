#include "CommandBuffer.h"

#include "CommandPool.h"

CommandBuffer::CommandBuffer()
{
}

CommandBuffer::CommandBuffer(LogicalDevice* Device, CommandPool* Pool, VkCommandBuffer Buffer)
	:
	mDevice(Device),
	mPool(Pool),
	mBuffer(Buffer)
{
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags Flags)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = Flags;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(mBuffer, &beginInfo) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to begin recording command buffer");
	}
}

void CommandBuffer::End()
{
	if (vkEndCommandBuffer(mBuffer) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to end recording command buffer");
	}
}

void CommandBuffer::EndWithSubmit()
{
	vkEndCommandBuffer(mBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mBuffer;

	// Create a fence to wait for the GPU to finish execution
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	
	auto device = Renderer::Get()->GetDevice();

	GPUFence* fence = GPUFence::Create(device);

	vkQueueSubmit(device->GetGraphicsQueue().queue, 1, &submitInfo, *fence);

	// Wait for the fence - i.e., wait for the command buffer to finish on GPU
	fence->Wait();

	fence->Reset();

	delete fence;
}

void CommandBuffer::Reset(VkCommandBufferResetFlags Flags)
{
	vkResetCommandBuffer(mBuffer, Flags);
}