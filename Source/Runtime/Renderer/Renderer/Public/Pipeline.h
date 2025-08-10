#pragma once
#include <Core.h>

class Pipeline
{
public:
	Pipeline(VkPipeline Pipeline, VkPipelineLayout Layout);
	~Pipeline();

	//void Bind();

	VkPipelineLayout GetLayout() { return mLayout; }

public:
	operator VkPipeline() const noexcept { return mPipeline; }

private:
	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
};