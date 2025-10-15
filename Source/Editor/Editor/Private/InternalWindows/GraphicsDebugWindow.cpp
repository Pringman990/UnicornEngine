#include "pch.h"
#include "GraphicsDebugWindow.h"

#include <imgui_impl_vulkan.h>
#include <Renderer.h>

GraphicsDebugWindow::GraphicsDebugWindow(Editor* EditorPtr) 
    :
    EditorWindow(EditorPtr)
{
    mWindowDisplayName = "Graphics Debug";
}

GraphicsDebugWindow::~GraphicsDebugWindow()
{
}

bool GraphicsDebugWindow::Init()
{
   // mGraphicsCardInfo = Renderer::Get()->GetGraphicsCardInfo();
   // ResourceHandle<Texture2D> textureHandle = AssetManager::Get()->LoadAsset<Texture2D>("engine://Test.png");
    //texture = AssetManager::Get()->GetAsset(textureHandle);

    //GPUTexture* gpuTexture = Renderer::Get()->GetGPUResourceManager().GetResource(texture->GetResourceData().gpuHandle);

    //sampler = Sampler::Create();
   // imguiTex = (ImTextureID)ImGui_ImplVulkan_AddTexture(*sampler, gpuTexture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    //mEditor->AddTextureToImgui(texture);

    return true;
}

void GraphicsDebugWindow::Render()
{
    //ImGui::Text("Texture Load Test:");
    //ImGui::Image(texture->GetImGuiHandle(), ImVec2(texture->GetMetadata().extent.x, texture->GetMetadata().extent.y));

    ImGui::Text(("FPS: " + std::to_string(GET_TIMER()->GetFps())).c_str());

    GraphicsCardInformation info = SubsystemManager::Get<Renderer>()->GetCardInfo();
    ImGui::Text(info.name.c_str());

    std::string vram = "Video Memory: ";
    vram += std::to_string(info.currentVideoMemoryUsage) + "/" + std::to_string(info.totalVideoMemoryInMB) + "(Free: " + std::to_string(info.approxFreeVideoMemory) + ")";

    ImGui::Text(vram.c_str());

    std::string sysMem = "System Memory: ";
    sysMem += std::to_string(info.systemMemoryInMB);
    ImGui::Text(sysMem.c_str());

    std::string sharedMem = "Shared Memory: ";
    sharedMem += std::to_string(info.sharedMemoryInMB);
    ImGui::Text(sharedMem.c_str());
}
