#include "pch.h"
#include "GraphicsDebugWindow.h"

#include <imgui_impl_vulkan.h>
#include <Sampler.h>
#include <GPUBarrier.h>
#include <Texture2D.h>
#include <AssetManager/AssetManager.h>

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
    ResourceHandle<Texture2D> textureHandle = AssetManager::Get()->LoadAsset<Texture2D>("engine://Test.png");
    texture = AssetManager::Get()->GetAsset(textureHandle);

    //GPUTexture* gpuTexture = Renderer::Get()->GetGPUResourceManager().GetResource(texture->GetResourceData().gpuHandle);

    //sampler = Sampler::Create();
   // imguiTex = (ImTextureID)ImGui_ImplVulkan_AddTexture(*sampler, gpuTexture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    mEditor->AddTextureToImgui(texture);

    return true;
}

void GraphicsDebugWindow::Render()
{
    ImGui::Text("Texture Load Test:");
    ImGui::Image(texture->GetImGuiHandle(), ImVec2(texture->GetMetadata().extent.x, texture->GetMetadata().extent.y));

    //mGraphicsCardInfo = Renderer::Get()->GetGraphicsCardInfo();
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
