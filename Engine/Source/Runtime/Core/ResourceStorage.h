#pragma once
#include "GenerationHandle.h"

template <typename T>
class ResourceStorage
{
public:
    ResourceStorage() = default;
    ~ResourceStorage() = default;

    GenerationHandle<T> Allocate(T&& resource)
    {
        GenerationHandle<T> handle;
        if (!mFreeSlots.empty())
        {
            auto index = mFreeSlots.back();
            mFreeSlots.pop_back();

            handle.generation = mResources[index].generation;
            handle.index = index;
            mResources[index].value = std::move(resource);
        }
        else
        {
            int32 index = mResources.size();
            handle.index = index;
            handle.generation = 1;
            mResources.push_back({.generation = handle.generation, .value = std::move(resource)});
        }

        return handle;
    }

    GenerationHandle<T> Allocate()
    {
        return Allocate(T());
    }

    T* Get(const GenerationHandle<T> handle)
    {
        if (handle.index >= mResources.size())
            return nullptr;

        auto& slot = mResources[handle.index];

        if (slot.generation != handle.generation)
            return nullptr;

        return &slot.value;
    }

    const T* Get(const GenerationHandle<T> handle) const
    {
        if (handle.index >= mResources.size())
            return nullptr;

        auto& slot = mResources[handle.index];

        if (slot.generation != handle.generation)
            return nullptr;

        return &slot.value;
    }

    void NullResources(GenerationHandle<T> handle)
    {
        if (handle.index >= mResources.size())
            return;

        auto& slot = mResources[handle.index];
        ++slot.generation;
        mFreeSlots.push_back(handle.index);
    }

private:
    struct ResourceSlot
    {
        uint32 generation = 0;
        T value;
    };

    List<ResourceSlot> mResources;
    List<int32> mFreeSlots;
};
