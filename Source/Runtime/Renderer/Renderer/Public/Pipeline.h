#pragma once
#include <Core.h>

class Pipeline
{
public:
	Pipeline(VkPipeline Pipeline, VkPipelineLayout Layout);
	~Pipeline();

	//void Bind();

public:
	operator VkPipeline() const noexcept { return mPipeline; }

private:
	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
};