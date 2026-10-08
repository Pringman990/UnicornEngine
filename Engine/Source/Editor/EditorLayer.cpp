#include "pch.h"
#include "EditorLayer.h"
#include <imgui.h>

void EditorLayer::BeginFrame()
{
    mBackend.BeginFrame();
    mWindows.Tick();
}

bool EditorLayer::OnInput(const InputEvent& event)
{
    const auto& io = ImGui::GetIO();
    return event.IsMouse() ? io.WantCaptureMouse : io.WantCaptureKeyboard;
}

void EditorLayer::Render()
{
    mBackend.RenderFrame();
    mBackend.EndFrame();
}
