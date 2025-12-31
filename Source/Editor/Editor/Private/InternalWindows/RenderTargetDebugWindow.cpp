#include "RenderTargetDebugWindow.h"

#include <Renderer.h>
#include <GPUResources/GPUTextureManager.h>

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(RenderTargetDebugWindow, "92b8e2bc-a1bb-49aa-8509-a15b46afad3c");

RenderTargetDebugWindow::RenderTargetDebugWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
}

RenderTargetDebugWindow::~RenderTargetDebugWindow()
{
}

bool RenderTargetDebugWindow::Init()
{
    return true;
}

void RenderTargetDebugWindow::Render()
{
	auto objectTexture = Renderer::Instance()->GetObjectIDVisualTexture();
	auto gpuObjectTexture = Renderer::Instance()->GetGPUTextureManager()->GetInternalTexture(objectTexture);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image(gpuObjectTexture->srv.Get(), ImVec2(256, 256));
}
