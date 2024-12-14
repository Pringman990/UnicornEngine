#pragma once
#include <Buffers/IRenderBufferFactory.h>

class DX11RenderBufferFactory : public IRenderBufferFactory
{
public:
	DX11RenderBufferFactory();
	~DX11RenderBufferFactory() override;

	virtual RenderBuffer* CreateVertexBuffer(const std::vector<Vertex>& aVertexArray) override;
	virtual RenderBuffer* CreateIndexBuffer(const std::vector<uint32>& aIndexArray) override;

private:

};