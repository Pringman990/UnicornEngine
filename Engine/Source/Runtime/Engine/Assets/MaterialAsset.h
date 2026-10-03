#pragma once

#include "Asset.h"
#include "Renderer/Renderer.h"

class ShaderAsset;
class TextureAsset;
class MaterialAssetSerializer;

struct AssetMaterialParameter
{
    String name;
    MaterialUniformValue value = 0.0f;
};

class MaterialAsset final : public Asset
{
    INIT_ASSET("Material")

public:
    ~MaterialAsset() override;

    AssetRef<ShaderAsset> GetShader() const { return mShaderReference; }
    const List<AssetRef<TextureAsset>>& GetTextures() const { return mTextureReferences; }
    const List<AssetMaterialParameter>& GetParameters() const { return mParameters; }
    void SetShader(AssetRef<ShaderAsset> shader) { mShaderReference = shader; }
    void SetTextures(List<AssetRef<TextureAsset>> textures) { mTextureReferences = std::move(textures); }
    void SetParameters(List<AssetMaterialParameter> parameters) { mParameters = std::move(parameters); }
    void SetParameter(const String& name, const MaterialUniformValue& value);
    MaterialHandle GetRuntimeHandle() const { return mRuntime; }

private:
    friend class MaterialAssetSerializer;

    AssetRef<ShaderAsset> mShaderReference;
    List<AssetRef<TextureAsset>> mTextureReferences;
    List<AssetMaterialParameter> mParameters;
    MaterialHandle mRuntime{};
    SharedPtr<const ShaderAsset> mShader;
    List<SharedPtr<const TextureAsset>> mTextures;
};
