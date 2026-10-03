#include "pch.h"
#include "IAssetSerializer.h"
#include "AssetFile.h"
#include "Engine/FileSystem.h"

Expected<OwnedPtr<Asset>, String> IAssetSerializer::Load(const AssetLoadRequest& request)
{
    if (!mArchives)
        return Unexpected(String("No archive factory configured"));

    auto bytes = Services::Get<FileSystem>().ReadAll(request.info.path);
    if (!bytes)
        return Unexpected(bytes.error().message);
    if (bytes->empty())
        return Unexpected(String("Asset descriptor is empty"));

    auto archive = mArchives(*bytes);
    if (!archive)
        return Unexpected(String("Archive creation failed"));
    auto info = ReadAssetInfo(*archive, request.info.path);
    if (!info)
        return Unexpected(info.error());
    if (info->id != request.info.id || info->type != request.info.type)
        return Unexpected(String("Asset descriptor identity does not match registration"));

    return Deserialize(request, *archive);
}

Expected<void, String> IAssetSerializer::Save(const Asset& asset, const AssetInfo& info)
{
    if (!mArchives)
        return Unexpected(String("No archive factory configured"));

    auto archive = mArchives({});
    if (!archive)
        return Unexpected(String("Archive creation failed"));
    auto metadata = WriteAssetInfo(*archive, info);
    if (!metadata)
        return metadata;
    auto result = Serialize(asset, *archive);
    if (!result)
        return result;

    // Archive owns format-specific file output and error reporting.
    archive->WriteToFile(info.path);
    return {};
}
