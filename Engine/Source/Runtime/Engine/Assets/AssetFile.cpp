#include "pch.h"
#include "AssetFile.h"

Expected<AssetInfo, String> ReadAssetInfo(Archive& archive, const String& path)
{
    uint32 version = 0;
    String id;
    AssetInfo info;
    if (!archive.ReadUInt32(version, "version") || version != 1 ||
        !archive.ReadString(id, "id") || !archive.ReadString(info.type, "type"))
        return Unexpected(String("Invalid asset metadata or unsupported version"));

    try
    {
        info.id = AssetID(id);
    }
    catch (...)
    {
        return Unexpected(String("Invalid asset UUID"));
    }
    if (!info.id.IsValid() || info.type.empty())
        return Unexpected(String("Asset ID and type must be valid"));

    info.path = path;
    return info;
}

Expected<void, String> WriteAssetInfo(Archive& archive, const AssetInfo& info)
{
    if (!archive.WriteUInt32(1, "version") || !archive.WriteString(info.id.ToString(), "id") ||
        !archive.WriteString(info.type, "type"))
        return Unexpected(String("Failed to write asset metadata"));

    return {};
}

String AssetSourcePath(const String& descriptorPath, const String& source)
{
    if (source.find("://") != String::npos || Path(source).is_absolute())
        return source;

    const auto separator = descriptorPath.find_last_of("/\\");
    return separator == String::npos ? source : descriptorPath.substr(0, separator + 1) + source;
}
