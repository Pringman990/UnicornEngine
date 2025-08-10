#pragma once
#include <Core.h>

#include "VertexShader.h"

class RenderContext
{
public:
	void BindShader(const VkPipelineShaderStageCreateInfo& ShaderStage);

	void Draw();
private:

};