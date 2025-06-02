#pragma once
#include <Core.h>
#include <Vulkan/vulkan.h>

class VkRenderer
{
public:
	bool Init();
	void PreRender();
	void Render();
	void Present();

private:
	VkRenderer();
	~VkRenderer();
private:

};