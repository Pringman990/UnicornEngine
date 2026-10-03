#pragma once

#include "Asset.h"
#include "Renderer/Renderer.h"

class TextureAssetSerializer;

class TextureAsset final : public Asset
{
    INIT_ASSET("Texture")

public:
    ~TextureAsset() override;

    const String& GetSource() const { return mSource; }
    void SetSource(String source) { mSource = std::move(source); }
    TextureHandle GetRuntimeHandle() const { return mRuntime; }

private:
    friend class TextureAssetSerializer;

    String mSource;
    TextureHandle mRuntime{};
};
