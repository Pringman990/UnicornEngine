#pragma once
#include "IImguiBackend.h"



class Win32DX12ImguiBackend : public IImguiBackend
{
public:
	Win32DX12ImguiBackend();
	~Win32DX12ImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame(CommandBuffer* Buffer) override;

	void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void AddTextureToImgui(Texture2D* Texture, Sampler* Sampler) override;

private:
	void ResizeBackBuffer(Vector2 NewSize);
};