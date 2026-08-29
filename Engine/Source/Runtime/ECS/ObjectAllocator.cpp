#include "pch.h"
#include "ObjectAllocator.h"

ObjectAllocator::ObjectAllocator(const Refl::Type& type, const size_t count)
    :
    mType(type),
    mCount(count)
{
    mSize = type.size * count;
    mData = ::operator new(mSize, static_cast<std::align_val_t>(type.alignment));
    mAlive.resize(mCount, false);
}

ObjectAllocator::~ObjectAllocator()
{
    for (uint32 i = 0; i < mCount; i++)
    {
        if (mAlive[i])
        {
            mType.ops.destroy(Get(i));
        }
    }

    ::operator delete(mData, static_cast<std::align_val_t>(mType.alignment));
}

void* ObjectAllocator::Get(const uint32 index) const
{
    return (index * mType.size) + static_cast<byte*>(mData);
}

void ObjectAllocator::SetAlive(uint32 index, bool alive)
{
    mAlive[index] = alive;
}
