#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Shader.h"
#include "GPUTexture.h"


struct GPUMaterial
{
	struct TextureDesc
	{
		uint32 slot;
		GPUResourceHandle<GPUTexture> texture;
		ShaderStageBind stage;
	};

	GPUResourceHandle<ShaderProgram> shaderProgram;
	Vector<TextureDesc> boundTextures;
};