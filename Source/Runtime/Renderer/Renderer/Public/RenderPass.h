#pragma once
#include <Core.h>

class RenderPass
{
public:
	RenderPass();
	~RenderPass();

	static RenderPass Create();

private:
	VkRenderPass mPass;
};