#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUMesh.h"

class BasicPrimitiveFactory
{
public:
	static GPUResourceHandle<GPUMesh> CreateCube();


private:
	BasicPrimitiveFactory() = delete;
	~BasicPrimitiveFactory() = delete;

};