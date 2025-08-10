#include "pch.h"
#include "IndexBuffer.h"

#include "CommandPool.h"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

IndexBuffer IndexBuffer::Create(const Vector<uint32>& Indicies, CommandPool* Pool)
{
	auto renderer = Renderer::Get();
	VkDevice device = *renderer->GetDevice();

	VkDeviceSize bufferSize = sizeof(Indicies[0]) * Indicies.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, Indicies.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	IndexBuffer buffer;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer.mBuffer, buffer.mMemory);

	CopyBuffer(Pool, stagingBuffer, buffer.mBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	return buffer;
}

void IndexBuffer::Free(LogicalDevice* Device)
{
	vkDestroyBuffer(*Device, mBuffer, nullptr);
	vkFreeMemory(*Device, mMemory, nullptr);
}

void IndexBuffer::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory)
{
	auto renderer = Renderer::Get();
	VkDevice device = *renderer->GetDevice();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = Size;
	bufferInfo.usage = Usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &Buffer) != VK_SUCCESS)
	{
		_THROW_RENDERER(false, "Failed to create buffer");
		return;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = renderer->GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, Properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &BufferMemory) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to allocate buffer memory");
		return;
	}

	vkBindBufferMemory(device, Buffer, BufferMemory, 0);
}

void IndexBuffer::CopyBuffer(CommandPool* Pool, VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
	auto renderer = Renderer::Get();
	LogicalDevice* device = renderer->GetDevice();

	CommandBuffer* buffer = Pool->AllocateCommandBuffer();
	buffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = Size;
	vkCmdCopyBuffer(*buffer, SrcBuffer, DstBuffer, 1, &copyRegion);

	buffer->End();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = buffer->GetAdressOf();

	vkQueueSubmit(device->GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->GetGraphicsQueue().queue);

	Pool->FreeCommandBuffer(buffer);
}
