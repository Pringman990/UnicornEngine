#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUBuffer.h"
#include "Vertex.h"


class Renderer;

class RenderBufferManager
{
public:
	RenderBufferManager(Renderer* InRenderer);
	~RenderBufferManager();

	DirectResourceHandle<GPUBuffer> CreateVertex(const Vector<Vertex>& Verticies);
	DirectResourceHandle<GPUBuffer> CreateIndex(const Vector<uint32>& Indicies);

	/*
	* Initial data can be null if you want to set the data at a later stage.
	* If the Initial data is set here then a Upload will take place directly
	*/
	DirectResourceHandle<GPUConstantBuffer> CreateConstantBuffer(uint32 Size, void* InitialData, BufferUsage Usage = BufferUsage::Default);

	void UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data);
	void BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStage Stages);
	void BindConstantBuffers(Vector<DirectResourceHandle<GPUConstantBuffer>> Buffers, uint32 StartSlot, ShaderStage Stages);

private:
	Renderer* mRenderer;
	DirectResourcePool<GPUBuffer> mBufferPool;
	DirectResourcePool<GPUConstantBuffer> mConstantBufferPool;
};