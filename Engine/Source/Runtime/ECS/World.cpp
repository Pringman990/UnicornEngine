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
    // Release every component before recycling the entity handle.
    auto& registry = refl::GetRegistry();
    for (auto& [typeId, storage] : mStorages)
    {
        const auto it = storage->entityToIndex.find(entity);
        if (it == storage->entityToIndex.end())
            continue;

        const uint32 index = it->second;
        registry.GetType(typeId).ops.destroy(storage->allocator->Get(index));
        storage->allocator->SetAlive(index, false);
        storage->freeIndexes.push_back(index);
        storage->entityToIndex.erase(it);
    }
    mEntityManager.Destroy(entity);
}
