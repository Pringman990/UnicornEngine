#pragma once

/**
 * Entity is split up in index and generation with memory layout:
 *
 * 63........32 31........0
 * Generation      Index
 */
using Entity = uint64;

constexpr Entity InvalidEntity = 0;

class EntityManager
{
public:
    EntityManager() = default;
    ~EntityManager() = default;

    Entity Create();
    void Destroy(Entity entity);

    NODISC const List<Entity>& GetEntities() const { return mEntities; };

private:
    List<Entity> mEntities;
    List<uint32> mFreeList;
};
