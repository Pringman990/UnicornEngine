#include "pch.h"
#include "GPUBufferManager.h"

#include "Renderer.h"

GPUBufferManager::GPUBufferManager(Renderer& RendererRef)
    :
    mRenderer(RendererRef)
{
}

GPUBufferManager::~GPUBufferManager()
{
}

//IndexBuffer GPUBufferManager::CreateIndexBuffer(const Vector<uint32>& Indices)
//{
//    return IndexBuffer();
//}

//VertexBuffer GPUBufferManager::CreateVertexBuffer(const Vector<Vertex>& Verticies)
//{
//	
//	VkDeviceSize bufferSize = sizeof(Verticies[0]) * Verticies.size();
//
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//	void* data;
//	vkMapMemory(*mRenderer.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
//	memcpy(data, Verticies.data(), (size_t)bufferSize);
//	vkUnmapMemory(*mRenderer.GetDevice(), stagingBufferMemory);
//
//	VulkanVertexBuffer vkBuffer;
//	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkBuffer.buffer, vkBuffer.memory);
//
//
//
//	VertexBuffer buffer;
//	buffer.mHandle = mVertexBufferPool.Add(vkBuffer);
//
//    return VertexBuffer();
//}
//
//void GPUBufferManager::FreeIndexBuffer(IndexBuffer& Buffer)
//{
//    mIndexBufferPool.Remove(Buffer.mHandle, [&](VulkanIndexBuffer& InternalBuffer)
//        {
//            vkDestroyBuffer(*mRenderer.GetDevice(), InternalBuffer.buffer, nullptr);
//            vkFreeMemory(*mRenderer.GetDevice(), InternalBuffer.memory, nullptr);
//        });
//}
//
//void GPUBufferManager::FreeVertexBuffer(VertexBuffer& Buffer)
//{
//    mVertexBufferPool.Remove(Buffer.mHandle, [&](VulkanVertexBuffer& InternalBuffer)
//        {
//            vkDestroyBuffer(mRenderer.GetDevice(), InternalBuffer.buffer, nullptr);
//            vkFreeMemory(mRenderer.GetDevice(), InternalBuffer.memory, nullptr);
//        });
//}
//
//void GPUBufferManager::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory)
//{
//	VkBufferCreateInfo bufferInfo{};
//	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	bufferInfo.size = Size;
//	bufferInfo.usage = Usage;
//	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//	if (vkCreateBuffer(mRenderer.GetDevice(), &bufferInfo, nullptr, &Buffer) != VK_SUCCESS)
//	{
//		_THROW_RENDERER(false, "Failed to create buffer");
//		return;
//	}
//
//	VkMemoryRequirements memRequirements;
//	vkGetBufferMemoryRequirements(mRenderer.GetDevice(), Buffer, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memRequirements.size;
//	allocInfo.memoryTypeIndex = mRenderer.FindMemoryType(memRequirements.memoryTypeBits, Properties);
//
//	if (vkAllocateMemory(mRenderer.GetDevice(), &allocInfo, nullptr, &BufferMemory) != VK_SUCCESS)
//	{
//		_THROW_RENDERER("Failed to allocate buffer memory");
//		return;
//	}
//
//	vkBindBufferMemory(mRenderer.GetDevice(), Buffer, BufferMemory, 0);
//}
//
//void GPUBufferManager::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
//{
//	VkCommandBufferAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = mCommandPool;
//	allocInfo.commandBufferCount = 1;
//
//	VkCommandBuffer commandBuffer;
//	vkAllocateCommandBuffers(mRenderer.GetDevice(), &allocInfo, &commandBuffer);
//
//	VkCommandBufferBeginInfo beginInfo{};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//	vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//	VkBufferCopy copyRegion{};
//	copyRegion.srcOffset = 0; // Optional
//	copyRegion.dstOffset = 0; // Optional
//	copyRegion.size = Size;
//	vkCmdCopyBuffer(commandBuffer, SrcBuffer, DstBuffer, 1, &copyRegion);
//
//	vkEndCommandBuffer(commandBuffer);
//
//	VkSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &commandBuffer;
//
//	vkQueueSubmit(mRenderer.GetDevice().GetGraphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(mRenderer.GetDevice().GetGraphicsQueue().queue);
//
//	vkFreeCommandBuffers(mRenderer.GetDevice(), mCommandPool, 1, &commandBuffer);
//}
