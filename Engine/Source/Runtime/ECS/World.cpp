#include "pch.h"
#include "World.h"

Entity World::CreateEntity()
{
    Entity entity = mEntityManager.Create();
    AddComponent<Relationship>(entity);
    auto* nameComponent = AddComponent<NameComponent>(entity);
    nameComponent->name = std::to_string(entity);
    return entity;
}

Entity World::CreateEntity(const String& name)
{
    const Entity entity = mEntityManager.Create();
    AddComponent<Relationship>(entity);
    auto* nameComponent = AddComponent<NameComponent>(entity);
    nameComponent->name = name.empty() ? std::to_string(entity) : name;
    return entity;
}

void World::DestroyEntity(const Entity entity)
{
    // TODO: needs to remove components also.
    mEntityManager.Destroy(entity);
}
