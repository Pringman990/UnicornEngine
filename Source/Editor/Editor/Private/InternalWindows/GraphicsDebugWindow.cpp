#include "pch.h"
#include "GraphicsDebugWindow.h"



GraphicsDebugWindow::GraphicsDebugWindow()
{
    mWindowDisplayName = "Graphics Debug";
}

GraphicsDebugWindow::~GraphicsDebugWindow()
{
}

bool GraphicsDebugWindow::Init()
{
   // mGraphicsCardInfo = Renderer::GetInstance()->GetGraphicsCardInfo();
    return true;
}

void GraphicsDebugWindow::Render()
{
    //mGraphicsCardInfo = Renderer::GetInstance()->GetGraphicsCardInfo();
    //ImGui::Text(mGraphicsCardInfo.name.c_str());

    //std::string vram = "Video Memory: ";
    //vram += std::to_string(mGraphicsCardInfo.currentVideoMemoryUsage) + "/" + std::to_string(mGraphicsCardInfo.totalVideoMemoryInMB) + "(Free: " + std::to_string(mGraphicsCardInfo.approxFreeVideoMemory) + ")";

    //ImGui::Text(vram.c_str());

    //std::string sysMem = "System Memory: ";
    //sysMem += std::to_string(mGraphicsCardInfo.systemMemoryInMB);
    //ImGui::Text(sysMem.c_str());

    //std::string sharedMem = "Shared Memory: ";
    //sharedMem += std::to_string(mGraphicsCardInfo.sharedMemoryInMB);
    //ImGui::Text(sharedMem.c_str());
}
