#include "pch.h"
#include "EntityManager.h"

Entity EntityManager::Create()
{
    Entity e{};

    if (!mFreeList.empty())
    {
        const uint32 index = mFreeList.back();
        e = mEntities[index];
        mFreeList.pop_back();
    }
    else
    {
        constexpr uint32 generation = 1;
        const uint32 index = mEntities.size();
        e = ToUint64(index, generation);
        mEntities.push_back(e);
    }

    return e;
}

void EntityManager::Destroy(const Entity entity)
{
    uint32 index{};
    uint32 generation{};
    FromUint64(entity, index, generation);

    generation++;

    mEntities[index] = ToUint64(index, generation);

    mFreeList.push_back(index);
}
