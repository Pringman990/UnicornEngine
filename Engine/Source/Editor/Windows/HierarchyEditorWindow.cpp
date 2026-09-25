//
// Created on 2026-08-29.
//

#include "pch.h"
#include "HierarchyEditorWindow.h"

#include "Editor.h"
#include "imgui.h"
#include "ECS/WorldManager.h"

HierarchyEditorWindow::HierarchyEditorWindow(Editor& editor, const char* name)
    :
    EditorWindowBase(editor, name)
{
}

HierarchyEditorWindow::~HierarchyEditorWindow()
{
}

bool HierarchyEditorWindow::Init()
{
    return true;
}

void HierarchyEditorWindow::Tick()
{
    auto& worldManager = Services::Get<WorldManager>();
    auto& world = worldManager.GetActiveWorld();

    UnorderedMap<Entity, List<Entity>> children;
    for (const auto& [entity, relation] : world.Query<Relationship>())
    {
        children[relation.parent].push_back(entity);
    }

    for (const auto& [entity, relation] : world.Query<Relationship>())
    {
        // We only want to process Root entities
        if (relation.parent != InvalidEntity)
            continue;

        DrawEntity(world, entity, children);
    }
}

void HierarchyEditorWindow::DrawEntity(World& world, Entity entity, const UnorderedMap<Entity, List<Entity>>& children)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

    auto it = children.find(entity);
    if (it == children.end())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (mEditor.IsEntitySelected(entity))
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    auto* nameComponent = world.GetComponent<NameComponent>(entity);
    bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, "%s", nameComponent->name.c_str());

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        mEditor.ClearSelectedEntities();
        mEditor.SelectEntity(entity);
    }

    if (open)
    {
        if (it != children.end())
        {
            for (Entity child : it->second)
            {
                DrawEntity(world, child, children);
            }
        }

        ImGui::TreePop();
    }
}
