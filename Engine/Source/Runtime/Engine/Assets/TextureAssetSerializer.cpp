#include "pch.h"
#include "TextureAssetSerializer.h"
#include "TextureAsset.h"
#include "AssetFile.h"
#include "Engine/FileSystem.h"
#include "Engine/TextureLoader.h"

Expected<OwnedPtr<Asset>, String> TextureAssetSerializer::Deserialize(const AssetLoadRequest& request, Archive& archive)
{
    TextureAsset data;
    if (!archive.ReadString(data.mSource, "source"))
        return Unexpected(String("Texture source is missing"));

    return Create(data, request);
}

Expected<OwnedPtr<Asset>, String> TextureAssetSerializer::Create(const Asset& data, const AssetLoadRequest& request)
{
    if (data.GetType() != TextureAsset::TYPE)
        return Unexpected(String("Expected a texture asset"));

    const auto& texture = static_cast<const TextureAsset&>(data);
    auto bytes = Services::Get<FileSystem>().ReadAll(AssetSourcePath(request.info.path, texture.mSource));
    if (!bytes)
        return Unexpected(bytes.error().message);

    const auto image = TextureLoader::LoadTexture(*bytes);
    if (image.data.empty() || image.width == 0 || image.height == 0)
        return Unexpected(String("Texture decode failed: ") + texture.mSource);
    if (image.channels != 1 && image.channels != 3 && image.channels != 4)
        return Unexpected(String("The renderer supports 1, 3, or 4 channel textures"));

    auto asset = MakeOwned<TextureAsset>();
    asset->mSource = texture.mSource;
    asset->mRuntime = Services::Get<Renderer>().CreateTexture({image.data, image.channels, image.width, image.height});
    return OwnedPtr<Asset>(std::move(asset));
}

Expected<void, String> TextureAssetSerializer::Serialize(const Asset& asset, Archive& archive)
{
    if (asset.GetType() != TextureAsset::TYPE)
        return Unexpected(String("Expected a texture asset"));
    if (!archive.WriteString(static_cast<const TextureAsset&>(asset).mSource, "source"))
        return Unexpected(String("Failed to write texture source"));

    return {};
}
