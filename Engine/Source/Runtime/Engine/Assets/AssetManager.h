#pragma once

#include "IAssetSerializer.h"
#include "Core/Defines.h"
#include "Core/ServiceRegistry.h"

// Single threaded storage. Type IDs are opaque keys; serializers own all IO
// and type-specific behavior.
class AssetManager
{
    INIT_SERVICE(AssetManager)

public:
    Expected<void, String> RegisterSerializer(const AssetTypeID& type, OwnedPtr<IAssetSerializer> serializer);
    Expected<void, String> Register(const AssetInfo& info);
    Expected<AssetHandle<>, String> Load(const AssetID& id);
    Expected<AssetHandle<>, String> Create(const AssetInfo& info, const Asset& data);
    Expected<void, String> Remove(AssetHandle<> handle);
    Expected<void, String> Reload(AssetHandle<> handle);
    Expected<void, String> Replace(AssetHandle<> handle, const Asset& edited);
    Expected<void, String> Save(AssetHandle<> handle);

    // Borrowed pointer: no ownership overhead in a render loop.
    const Asset* Get(AssetHandle<> handle) const;
    // Shared ownership: keep an exact version alive for dependencies/snapshots.
    SharedPtr<const Asset> Acquire(AssetHandle<> handle) const;
    uint64 GetRevision(AssetHandle<> handle) const;

    // Call after snapshots using raw pointers/runtime handles have finished.
    // Dependencies and snapshots holding shared ownership keep versions alive.
    void ReleaseRetired();
    void Shutdown();

    template<typename T>
    Expected<AssetHandle<T>, String> Load(AssetRef<T> reference)
    {
        const auto it = mLookup.find(reference.id);
        if (it == mLookup.end())
            return Unexpected(String("Asset is not registered"));
        if (mSlots[it->second].info.type != T::TYPE)
            return Unexpected(String("Asset type mismatch"));
        auto result = Load(reference.id);
        if (!result)
            return Unexpected(result.error());
        return AssetHandle<T>{result->index, result->generation};
    }

    template<typename T>
    const T* Get(AssetHandle<T> handle) const
    {
        const auto* asset = Get(AssetHandle<>{handle.index, handle.generation});
        return asset && asset->GetType() == T::TYPE ? static_cast<const T*>(asset) : nullptr;
    }

    template<typename T>
    SharedPtr<const T> Acquire(AssetHandle<T> handle) const
    {
        auto asset = Acquire(AssetHandle<>{handle.index, handle.generation});
        return asset && asset->GetType() == T::TYPE ? std::static_pointer_cast<const T>(asset) : nullptr;
    }

private:
    struct Slot
    {
        AssetInfo info;
        SharedPtr<const Asset> asset;
        uint64 revision = 0;
        bool loading = false;
        uint32 generation = 1;
    };

    Expected<void, String> LoadSlot(uint32 index, const Asset* edited = nullptr);
    bool IsLoaded(AssetHandle<> handle) const;

    List<Slot> mSlots;
    UnorderedMap<AssetID, uint32> mLookup;
    UnorderedMap<AssetTypeID, OwnedPtr<IAssetSerializer>> mSerializers;
    List<SharedPtr<const Asset>> mRetired;
};
