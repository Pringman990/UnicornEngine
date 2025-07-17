#pragma once
#include <Core.h>

#include "GPUBuffer.h"

class GenericGPUBuffer
{
public:
	GenericGPUBuffer();
	GenericGPUBuffer(GPUBuffer Buffer);
	~GenericGPUBuffer();

	static GenericGPUBuffer CreateOneTimeBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	static GenericGPUBuffer* Create(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	static SharedPtr<GenericGPUBuffer> CreateSingleFrameBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	
	void Map(const void* Data, VkDeviceSize Size);

	VkBuffer GetRaw() const { return mBuffer.buffer; };

private:
	GPUBuffer mBuffer;
};