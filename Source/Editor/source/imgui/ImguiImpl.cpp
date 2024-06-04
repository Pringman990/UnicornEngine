#include "EditorPch.h"
#include "ImguiImpl.h"

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>

#include <source/Window.h>
#include <source/DX11.h>

ImguiImpl::ImguiImpl() 
{
}

ImguiImpl::~ImguiImpl()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool ImguiImpl::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    if (!ImGui_ImplWin32_Init(GraphicsEngine::GetInstance().GetCurrentWindow().GetWindowInfo().currentWindow))
        return false;
    if (!ImGui_ImplDX11_Init(GraphicsEngine::GetInstance().GetDX11().GetDevice(), GraphicsEngine::GetInstance().GetDX11().GetDeviceContext()))
        return false;

    return true;
}

void ImguiImpl::BeginFrame()
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImguiImpl::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
}