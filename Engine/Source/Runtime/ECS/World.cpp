#include "pch.h"
#include "World.h"

Entity World::CreateEntity()
{
    Entity entity = mEntityManager.Create();
    AddComponent<Relationship>(entity);
    return entity;
}

void World::DestroyEntity(const Entity entity)
{
    mEntityManager.Destroy(entity);
}
