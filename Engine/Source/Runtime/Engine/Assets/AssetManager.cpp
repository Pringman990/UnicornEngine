#include "pch.h"
#include "AssetManager.h"

Expected<void, String> AssetManager::RegisterSerializer(const AssetTypeID& type, OwnedPtr<IAssetSerializer> serializer)
{
    if (type.empty() || !serializer)
        return Unexpected(String("Serializer and type must be valid"));
    if (mSerializers.contains(type))
        return Unexpected(String("Serializer is already registered: ") + type);
    mSerializers.emplace(type, std::move(serializer));
    return {};
}

Expected<void, String> AssetManager::Register(const AssetInfo& info)
{
    if (!info.id.IsValid() || info.type.empty())
        return Unexpected(String("Asset ID and type must be valid"));
    if (mLookup.contains(info.id))
        return Unexpected(String("Duplicate asset ID: ") + info.id.ToString());
    const auto index = static_cast<uint32>(mSlots.size());
    Slot slot;
    slot.info = info;
    mSlots.push_back(std::move(slot));
    mLookup.emplace(info.id, index);
    return {};
}

Expected<AssetHandle<>, String> AssetManager::Load(const AssetID& id)
{
    const auto it = mLookup.find(id);
    if (it == mLookup.end())
        return Unexpected(String("Asset is not registered: ") + id.ToString());
    const uint32 index = it->second;
    if (mSlots[index].loading)
        return Unexpected(String("Circular asset dependency: ") + id.ToString());
    if (!mSlots[index].asset)
    {
        auto result = LoadSlot(index);
        if (!result)
            return Unexpected(result.error());
    }
    return AssetHandle<>{index, mSlots[index].generation};
}

Expected<AssetHandle<>, String> AssetManager::Create(const AssetInfo& info, const Asset& data)
{
    if (data.GetType() != info.type)
        return Unexpected(String("Asset type mismatch"));

    auto registered = Register(info);
    if (!registered)
        return Unexpected(registered.error());

    const uint32 index = mLookup.at(info.id);
    auto result = LoadSlot(index, &data);
    if (!result)
    {
        mLookup.erase(info.id);
        return Unexpected(result.error());
    }

    return AssetHandle<>{index, mSlots[index].generation};
}

Expected<void, String> AssetManager::Remove(AssetHandle<> handle)
{
    if (!IsLoaded(handle))
        return Unexpected(String("Asset handle is stale or not loaded"));

    auto& slot = mSlots[handle.index];
    if (slot.loading)
        return Unexpected(String("Cannot remove an asset while loading"));

    mRetired.push_back(slot.asset);
    slot.asset.reset();
    mLookup.erase(slot.info.id);
    ++slot.generation;
    if (slot.generation == 0)
        ++slot.generation;

    return {};
}

Expected<void, String> AssetManager::LoadSlot(uint32 index, const Asset* edited)
{
    // Copy metadata: a serializer may register additional assets while loading.
    AssetLoadRequest request{mSlots[index].info, *this};
    const auto it = mSerializers.find(request.info.type);
    if (it == mSerializers.end())
        return Unexpected(String("No serializer registered for: ") + request.info.type);
    if (mSlots[index].loading)
        return Unexpected(String("Asset is already loading"));
    if (edited && edited->GetType() != request.info.type)
        return Unexpected(String("Replacement asset type mismatch"));

    mSlots[index].loading = true;
    try
    {
        auto result = edited ? it->second->Create(*edited, request) : it->second->Load(request);
        mSlots[index].loading = false;
        if (!result)
            return Unexpected(result.error());
        if (!*result || (*result)->GetType() != request.info.type)
            return Unexpected(String("Serializer returned an incorrect asset type"));
        SharedPtr<const Asset> version(std::move(*result));
        auto& slot = mSlots[index];
        if (slot.asset)
            mRetired.push_back(slot.asset);
        slot.asset = std::move(version);
        ++slot.revision;
        return {};
    }
    catch (const std::exception& error)
    {
        mSlots[index].loading = false;
        return Unexpected(String(error.what()));
    }
    catch (...)
    {
        mSlots[index].loading = false;
        return Unexpected(String("Asset serializer failed"));
    }
}

bool AssetManager::IsLoaded(AssetHandle<> handle) const
{
    return handle.index < mSlots.size() && handle.generation == mSlots[handle.index].generation && mSlots[handle.index].asset;
}

const Asset* AssetManager::Get(AssetHandle<> handle) const
{
    return IsLoaded(handle) ? mSlots[handle.index].asset.get() : nullptr;
}

SharedPtr<const Asset> AssetManager::Acquire(AssetHandle<> handle) const
{
    return IsLoaded(handle) ? mSlots[handle.index].asset : nullptr;
}

uint64 AssetManager::GetRevision(AssetHandle<> handle) const
{
    return IsLoaded(handle) ? mSlots[handle.index].revision : 0;
}

Expected<void, String> AssetManager::Reload(AssetHandle<> handle)
{
    if (!IsLoaded(handle))
        return Unexpected(String("Asset handle is stale or not loaded"));
    return LoadSlot(handle.index);
}

Expected<void, String> AssetManager::Replace(AssetHandle<> handle, const Asset& edited)
{
    if (!IsLoaded(handle))
        return Unexpected(String("Asset handle is stale or not loaded"));
    return LoadSlot(handle.index, &edited);
}

Expected<void, String> AssetManager::Save(AssetHandle<> handle)
{
    if (!IsLoaded(handle))
        return Unexpected(String("Asset handle is stale or not loaded"));
    const AssetInfo info = mSlots[handle.index].info;
    const auto it = mSerializers.find(info.type);
    if (it == mSerializers.end())
        return Unexpected(String("No serializer registered for: ") + info.type);
    auto asset = Acquire(handle);
    try
    {
        return it->second->Save(*asset, info);
    }
    catch (const std::exception& error)
    {
        return Unexpected(String(error.what()));
    }
    catch (...)
    {
        return Unexpected(String("Asset serializer failed"));
    }
}

void AssetManager::ReleaseRetired()
{
    mRetired.clear();
}

void AssetManager::Shutdown()
{
    ReleaseRetired();
    for (auto& slot : mSlots)
    {
        slot.asset.reset();
        slot.revision = 0;
        ++slot.generation;
        if (slot.generation == 0)
            ++slot.generation;
    }
}
