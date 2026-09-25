//
// Created on 2026-08-30.
//

#include "pch.h"
#include "InspectorEditorWindow.h"

#include "imgui.h"
#include "ECS/WorldManager.h"

InspectorEditorWindow::InspectorEditorWindow(Editor& editor, const char* name)
    :
    EditorWindowBase(editor, name)
{
}

InspectorEditorWindow::~InspectorEditorWindow()
{
}

bool InspectorEditorWindow::Init()
{

    return true;
}

void InspectorEditorWindow::Tick()
{
    auto& refl = Services::Get<refl::ReflectionRegistry>();
    auto& worldManager = Services::Get<WorldManager>();
    auto& world = worldManager.GetActiveWorld();

    if (mEditor.GetSelectedEntities().empty())
        return;

    auto selectedEntity = mEditor.GetSelectedEntities()[0];

    const auto& components = world.GetAllComponents(selectedEntity);
    for (auto& [typeId, data] : components)
    {
        auto& type = refl.GetType(typeId);
        ImGui::TextUnformatted(type.displayName.c_str());

        if (mEditor.HasTypeDrawer(typeId))
        {
            mEditor.DrawType(typeId, type.displayName, data);
        }
        else
        {
            for (auto& property : type.properties)
            {
                void* propertyObj = static_cast<byte*>(data) + property.offset;
                mEditor.DrawType(property.type->id, property.displayName, propertyObj);
            }
        }

        ImGui::Separator();
    }

    if (ImGui::BeginListBox("Components"))
    {
        auto* componentTypes = refl.GetAllTypesOfCategory("Component");
        for (auto& typeIndex : *componentTypes)
        {
            auto& type = refl.GetType(typeIndex);
            if (ImGui::Selectable(type.displayName.c_str()))
            {
                if (!world.HasComponent(selectedEntity, typeIndex))
                    world.AddComponent(selectedEntity, typeIndex);
            }
        }

        ImGui::EndListBox();
    }
}
