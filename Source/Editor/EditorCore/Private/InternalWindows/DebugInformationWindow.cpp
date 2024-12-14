#include "pch.h"
#include "DebugInformationWindow.h"

#include <Timer.h>
#include <Renderer.h>

DebugInformationWindow::DebugInformationWindow()
{
    mWindowDisplayName = "Debug Information";
}

DebugInformationWindow::~DebugInformationWindow()
{
}

bool DebugInformationWindow::Init()
{
    return true;
}

void DebugInformationWindow::Render()
{
    static bool vsync = Renderer::GetInstance()->IsVsyncEnabled();

    if (ImGui::Checkbox("Vsync", &vsync))
    {
        Renderer::GetInstance()->SetVsync(vsync);
    }

    std::string fps = "FPS: ";
    fps += std::to_string(Timer::GetInstance()->GetFps());
    ImGui::Text(fps.c_str());
}
