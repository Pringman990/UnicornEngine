#pragma once
#include "IImguiBackend.h"

class Win32VulkanImguiBackend : public IImguiBackend
{
public:
	Win32VulkanImguiBackend();
	~Win32VulkanImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;

private:
	void ResizeBackBuffer(Vector2 NewSize);
};