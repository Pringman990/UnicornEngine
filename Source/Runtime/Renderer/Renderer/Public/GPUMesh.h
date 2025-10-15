#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUBuffer.h"

struct GPUMesh
{
	struct Submesh
	{
		uint32 startIndex = 0;
		uint32 indexCount = 0;
		//GPUResourceHandle<Material> materialHandle;
	};
	Vector<Submesh> submeshes;

	DirectResourceHandle<GPUBuffer> vertexBuffer;
	DirectResourceHandle<GPUBuffer> indexBuffer;
};