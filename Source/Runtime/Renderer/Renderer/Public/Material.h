#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Shader.h"

enum class MaterialBlendTypes
{
	Opaque
};

struct Material
{
	GPUResourceHandle<Shader> shader;
	MaterialBlendTypes blend;
};