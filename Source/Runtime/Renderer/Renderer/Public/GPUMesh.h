#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUBuffer.h"
#include "Material.h"

struct GPUMesh
{
	struct Submesh
	{
		uint32 startIndex = 0;
		uint32 indexCount = 0;
		AssetHandle<Material> materialHandle;
	};
	Vector<Submesh> submeshes;

	DirectResourceHandle<GPUBuffer> vertexBuffer;
	DirectResourceHandle<GPUBuffer> indexBuffer;
};