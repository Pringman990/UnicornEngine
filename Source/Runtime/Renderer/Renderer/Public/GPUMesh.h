#pragma once
#include <Core.h>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

struct GPUMesh
{
	struct GPUSubMesh
	{
		uint32 indexOffset;
		uint32 indexCount;
	};

	Vector<GPUSubMesh> subMeshes;
	IndexBuffer indexBuffer;
	VertexBuffer vertexBuffer;

	static void Destroy(GPUMesh* Mesh, LogicalDevice* Device)
	{
		Mesh->indexBuffer.Free(Device);
		Mesh->vertexBuffer.Free(Device);
	}
};