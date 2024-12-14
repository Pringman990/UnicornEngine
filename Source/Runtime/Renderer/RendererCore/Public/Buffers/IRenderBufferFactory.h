#pragma once
#include "Buffers/RenderBuffer.h"
#include "Vertex.h"

class IRenderBufferFactory
{
public:
	IRenderBufferFactory() = default;
	virtual ~IRenderBufferFactory() = default;

	virtual RenderBuffer* CreateVertexBuffer(const std::vector<Vertex>& aVertexArray) = 0;
	virtual RenderBuffer* CreateIndexBuffer(const std::vector<uint32>& aIndexArray) = 0;

private:

};