#pragma once
#include "Core/ReflectionRegistry.h"

class ObjectAllocator
{
public:
    ObjectAllocator(const Refl::Type& type, size_t count);
    ~ObjectAllocator();

    NODISC void* Get(uint32 index) const;

    void SetAlive(uint32 index, bool alive);

    NODISC size_t GetCount() const { return mCount; };
    NODISC size_t GetSize() const { return mSize; };

    NODISC void* GetData() const { return mData; };

private:
    Refl::Type mType;
    void* mData;

    size_t mCount;
    size_t mSize;
    List<bool> mAlive;
};
