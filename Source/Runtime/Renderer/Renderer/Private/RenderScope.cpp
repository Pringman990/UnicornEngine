#include "RenderScope.h"

#include <vulkan/vulkan_core.h>

RenderScope::RenderScope(CommandBuffer* Buffer, RenderScopeInfo ScopeInfo)
	:
	mCommandBuffer(Buffer)
{
	vkCmdBeginRendering(*mCommandBuffer, &ScopeInfo.renderingInfo);
}

RenderScope::RenderScope(RenderScopeInfo ScopeInfo)
	:
	mCommandBuffer(Renderer::Get()->GetCurrentFrameSyncCommandBuffer())
{
	vkCmdBeginRendering(*mCommandBuffer, &ScopeInfo.renderingInfo);
}

RenderScope::~RenderScope()
{
	vkCmdEndRendering(*mCommandBuffer);
}
