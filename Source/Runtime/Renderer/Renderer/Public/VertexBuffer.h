#pragma once
#include <Core.h>
#include "Vertex.h"

class CommandPool;

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	static VertexBuffer Create(const Vector<Vertex3D>& Verticies, CommandPool* Pool);

	void Free(LogicalDevice* Device);

	VkBuffer* GetAdressOf() noexcept { return &mBuffer; };
	VkBuffer GetRaw() const noexcept { return mBuffer; };

public:
	operator VkBuffer() const noexcept { return mBuffer; }
private:
	static void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory);
	static void CopyBuffer(CommandPool* Pool, VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);
private:
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
};