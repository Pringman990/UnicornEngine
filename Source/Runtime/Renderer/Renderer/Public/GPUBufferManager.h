#pragma once
#include <Core.h>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

class Renderer;

class GPUBufferManager
{
public:
	GPUBufferManager(Renderer& RendererRef);
	~GPUBufferManager();

//	IndexBuffer CreateIndexBuffer(const Vector<uint32>& Indices);
//	VertexBuffer CreateVertexBuffer(const Vector<Vertex>& Verticies);
//
//	void FreeIndexBuffer(IndexBuffer& Buffer);
//	void FreeVertexBuffer(VertexBuffer& Buffer);
//
//private:
//	void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory);
//	void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);
private:
	Renderer& mRenderer;

	ResourcePool<IndexBuffer> mIndexBufferPool;
	ResourcePool<VertexBuffer> mVertexBufferPool;
};