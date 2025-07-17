#include "Pipeline.h"

Pipeline::Pipeline(VkPipeline Pipeline, VkPipelineLayout Layout)
    :
    mPipeline(Pipeline),
    mLayout(Layout)
{
    _ENSURE_RENDERER(mPipeline, "Pipeline is not valid");
    _ENSURE_RENDERER(mLayout, "Layout is not valid");
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(*Renderer::Get()->GetDevice(), mPipeline, nullptr);
    vkDestroyPipelineLayout(*Renderer::Get()->GetDevice(), mLayout, nullptr);
}
