#pragma once
#include <Core.h>

class CommandPool;

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	static IndexBuffer Create(const Vector<uint32>& Indicies, CommandPool* Pool);

	void Free();

private:
	static void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory);
	static void CopyBuffer(CommandPool* Pool, VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);
private:
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
};