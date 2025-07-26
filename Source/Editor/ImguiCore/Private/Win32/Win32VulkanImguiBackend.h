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
	virtual void EndFrame(CommandBuffer* Buffer) override;

	void AddTextureToImgui(Texture2D* Texture, Sampler* Sampler) override;
	ImTextureID AddTextureToImgui(GPUTexture* Texture, Sampler* Sample) override;

private:
	bool ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ResizeBackBuffer(Vector2 NewSize);
private:
};