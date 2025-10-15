#pragma once
#include <EngineMinimal.h>
#include "GPUTexture.h"

struct RenderTargetBinding
{
	Vector<GPUResourceHandle<GPUTexture>> renderTargets;
	GPUResourceHandle<GPUTexture> depthTarget;
};