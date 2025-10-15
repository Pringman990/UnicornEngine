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

	//void AddTextureToImgui(Texture2D* Texture, Sampler* Sampler) override;

private:
	bool ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ResizeBackBuffer(Vector2 NewSize);
private:
};