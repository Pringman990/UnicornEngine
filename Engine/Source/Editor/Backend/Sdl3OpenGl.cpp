//
// Created on 2026-08-30.
//

#include "pch.h"
#include "Sdl3OpenGl.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "../../Runtime/Engine/Application.h"
#include "Renderer/Renderer.h"

SDL3OpenGL::SDL3OpenGL()
{
}

SDL3OpenGL::~SDL3OpenGL()
{
}

bool SDL3OpenGL::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    const auto& renderer = Services::Get<Renderer>();
    auto& app = Services::Get<Application>();
    auto* appWindow = app.GetWindow();

    if (!ImGui_ImplSDL3_InitForOpenGL(appWindow, renderer.GetContext()))
        return false;

    if (!ImGui_ImplOpenGL3_Init("#version 460"))
        return false;

    app.mProcessEventsNotifier.AddRaw(this, &SDL3OpenGL::ProcessEvent);

    return true;
}

void SDL3OpenGL::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void SDL3OpenGL::RenderFrame()
{
    ImGui::Render();
}

void SDL3OpenGL::EndFrame()
{
    const auto& renderer = Services::Get<Renderer>();
    const auto& app = Services::Get<Application>();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(app.GetWindow(), renderer.GetContext());
}

void SDL3OpenGL::ProcessEvent(SDL_Event event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}
