#pragma once
#include "EntityManager.h"
#include "ObjectAllocator.h"

struct ComponentStorage
{
    OwnedPtr<ObjectAllocator> allocator;
    UnorderedMap<Entity, uint32> entityToIndex;
    List<uint32> freeIndexes;
};
