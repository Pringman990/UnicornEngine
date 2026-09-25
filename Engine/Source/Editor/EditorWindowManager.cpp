//
// Created on 2026-08-29.
//

#include "pch.h"
#include "EditorWindowManager.h"

#include "imgui.h"

EditorWindowManager::EditorWindowManager()
{
}

EditorWindowManager::~EditorWindowManager()
{
}

void EditorWindowManager::Init()
{
    for (auto& window : mWindows)
    {
        window->Init();
    }
    mInitialized = true;
}

void EditorWindowManager::Tick()
{
    for (auto& window : mWindows)
    {
        if (ImGui::Begin(window->GetName()))
        {
            window->Tick();
            ImGui::End();
        }
    }
}
