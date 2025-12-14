#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUBuffer.h"
#include "GPUResources/GPUMaterial.h"

struct GPUMesh
{
	DirectResourceHandle<GPUBuffer> vertexBuffer;
	DirectResourceHandle<GPUBuffer> indexBuffer;
};