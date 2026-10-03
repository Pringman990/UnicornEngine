#pragma once

#include "Asset.h"
#include "Core/Archive.h"

class AssetManager;

struct AssetLoadRequest
{
    AssetInfo info;
    AssetManager& assets;
};

// Supply a format outside the asset system. Empty bytes create a writing archive.
using AssetArchiveFactory = Func<OwnedPtr<Archive>(const ByteBuffer&)>;

class IAssetSerializer
{
public:
    explicit IAssetSerializer(AssetArchiveFactory archives = {}) : mArchives(std::move(archives)) {}
    virtual ~IAssetSerializer() = default;

    virtual Expected<OwnedPtr<Asset>, String> Load(const AssetLoadRequest& request);
    virtual Expected<void, String> Save(const Asset& asset, const AssetInfo& info);

    virtual Expected<OwnedPtr<Asset>, String> Deserialize(const AssetLoadRequest& request, Archive& archive) = 0;
    virtual Expected<void, String> Serialize(const Asset& asset, Archive& archive) = 0;

    virtual Expected<OwnedPtr<Asset>, String> Create(const Asset&, const AssetLoadRequest&)
    {
        return Unexpected(String("This serializer does not support creation from edited data"));
    }

private:
    AssetArchiveFactory mArchives;
};
