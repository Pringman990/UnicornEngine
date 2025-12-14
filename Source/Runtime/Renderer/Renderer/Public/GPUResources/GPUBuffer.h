#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

/*
* Mainly used for Index and Vertex buffers
*/
struct GPUBuffer
{
	ComPtr<ID3D11Buffer> buffer;
};

/**
* Usually a compile time defined constant buffer.  
*/
struct GPUConstantBuffer
{
	ComPtr<ID3D11Buffer> buffer;
	Vector<byte> cpuData;
	uint32 size = 0;
	BufferUsage usage;
};

/**
* Runtime constant buffer made from reflection data of shader bytecode.
*/
struct GPUGenericConstantBuffer
{
	ComPtr<ID3D11Buffer> buffer;
	Vector<byte> cpuData;
	uint32 size = 0;
	BufferUsage usage;
};