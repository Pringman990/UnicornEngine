#include "pch.h"
#include "DebugInformationWindow.h"

#include <Timer/Timer.h>
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
    //static bool vsync = Renderer::Get()->IsVsyncEnabled();

    //if (ImGui::Checkbox("Vsync", &vsync))
    //{
    //    Renderer::Get()->SetVsync(vsync);
    //}

    String fps = "FPS: ";
    fps += std::to_string(Timer::Get()->GetFps());
    ImGui::Text(fps.c_str());

    String drawcalls = "DrawCalls: ";
    drawcalls += std::to_string(Renderer::Get()->GetDrawCalls());
    ImGui::Text(drawcalls.c_str());
}
