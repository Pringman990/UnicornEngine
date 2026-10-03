#pragma once

#include "IAssetSerializer.h"

class ShaderAssetSerializer final : public IAssetSerializer
{
public:
    using IAssetSerializer::IAssetSerializer;

    Expected<OwnedPtr<Asset>, String> Deserialize(const AssetLoadRequest& request, Archive& archive) override;
    Expected<void, String> Serialize(const Asset& asset, Archive& archive) override;
    Expected<OwnedPtr<Asset>, String> Create(const Asset& data, const AssetLoadRequest& request) override;
};
