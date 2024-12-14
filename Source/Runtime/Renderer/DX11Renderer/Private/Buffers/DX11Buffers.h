#pragma once
#include <Buffers/RenderBuffer.h>

struct DX11VertexBuffer : public RenderBuffer
{
	void Release() override
	{
		mVertexBuffer->Release();
	}
	ID3D11Buffer* mVertexBuffer;
};

struct DX11IndexBuffer : public RenderBuffer
{
	void Release() override 
	{
		mIndexBuffer->Release();
	}
	ID3D11Buffer* mIndexBuffer;
};