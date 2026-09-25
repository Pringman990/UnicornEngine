#pragma once
#include "EditorWindowBase.h"
#include "ECS/EntityManager.h"
#include "ECS/World.h"

class HierarchyEditorWindow : public EditorWindowBase
{
public:
    HierarchyEditorWindow(Editor& editor, const char* name);
    ~HierarchyEditorWindow() override;

    bool Init() override;
    void Tick() override;
private:
    void DrawEntity(World& world, Entity entity, const UnorderedMap<Entity, List<Entity>>& children);
private:
};
