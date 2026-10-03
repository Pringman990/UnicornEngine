#include "pch.h"
#include "MaterialAssetSerializer.h"
#include "MaterialAsset.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"
#include "AssetManager.h"

namespace
{
    bool WriteValue(Archive& archive, float value)
    {
        return archive.WriteFloat(value, "");
    }

    template<glm::length_t L, glm::qualifier Q>
    bool WriteValue(Archive& archive, const glm::vec<L, float, Q>& value)
    {
        bool valid = true;
        for (glm::length_t i = 0; i < L; ++i)
            valid = archive.WriteFloat(value[i], "") && valid;

        return valid;
    }
}

Expected<OwnedPtr<Asset>, String> MaterialAssetSerializer::Deserialize(const AssetLoadRequest& request, Archive& archive)
{
    MaterialAsset data;
    String shader;
    if (!archive.ReadString(shader, "shader"))
        return Unexpected(String("Material shader is missing"));

    try
    {
        data.mShaderReference.id = AssetID(shader);
    }
    catch (...)
    {
        return Unexpected(String("Invalid material shader UUID"));
    }

    if (archive.HasKey("textures"))
    {
        if (!archive.BeginReadArray("textures"))
            return Unexpected(String("Invalid material texture list"));

        const uint32 count = archive.GetArraySize();
        bool valid = count <= MAX_TEXTURE_SLOTS;
        for (uint32 i = 0; i < count && valid; ++i)
        {
            String id;
            valid = archive.ReadString(id, "");
            if (valid)
            {
                try
                {
                    data.mTextureReferences.push_back({AssetID(id)});
                }
                catch (...)
                {
                    valid = false;
                }
            }
            archive.Next();
        }
        archive.EndReadArray();

        if (!valid)
            return Unexpected(String("Invalid material textures or renderer limit exceeded"));
    }

    if (archive.HasKey("parameters"))
    {
        if (!archive.BeginReadArray("parameters"))
            return Unexpected(String("Invalid material parameter list"));

        const uint32 count = archive.GetArraySize();
        bool valid = count <= MAX_MATERIAL_PARAMETERS;
        for (uint32 i = 0; i < count && valid; ++i)
        {
            if (!archive.BeginReadObject(""))
            {
                valid = false;
                break;
            }

            AssetMaterialParameter parameter;
            valid = archive.ReadString(parameter.name, "name");
            if (valid && archive.BeginReadArray("value"))
            {
                const uint32 components = archive.GetArraySize();
                valid = components >= 1 && components <= 4;
                float values[4]{};
                for (uint32 v = 0; v < components && valid; ++v)
                {
                    valid = archive.ReadFloat(values[v], "");
                    archive.Next();
                }
                archive.EndReadArray();

                if (valid)
                {
                    switch (components)
                    {
                    case 1: parameter.value = values[0]; break;
                    case 2: parameter.value = glm::vec2(values[0], values[1]); break;
                    case 3: parameter.value = glm::vec3(values[0], values[1], values[2]); break;
                    case 4: parameter.value = glm::vec4(values[0], values[1], values[2], values[3]); break;
                    }
                    data.mParameters.push_back(std::move(parameter));
                }
            }
            else
                valid = false;

            archive.EndReadObject();
            archive.Next();
        }
        archive.EndReadArray();

        if (!valid)
            return Unexpected(String("Invalid material parameters or renderer limit exceeded"));
    }

    return Create(data, request);
}

Expected<OwnedPtr<Asset>, String> MaterialAssetSerializer::Create(const Asset& data, const AssetLoadRequest& request)
{
    if (data.GetType() != MaterialAsset::TYPE)
        return Unexpected(String("Expected a material asset"));

    const auto& material = static_cast<const MaterialAsset&>(data);
    if (material.mTextureReferences.size() > MAX_TEXTURE_SLOTS || material.mParameters.size() > MAX_MATERIAL_PARAMETERS)
        return Unexpected(String("Material exceeds renderer limits"));

    auto shader = request.assets.Load(material.mShaderReference);
    if (!shader)
        return Unexpected(shader.error());

    auto asset = MakeOwned<MaterialAsset>();
    asset->mShaderReference = material.mShaderReference;
    asset->mTextureReferences = material.mTextureReferences;
    asset->mParameters = material.mParameters;
    asset->mShader = request.assets.Acquire(*shader);
    MaterialCreateInfo info{};
    info.shaderProgram = asset->mShader->GetRuntimeHandle();
    for (const auto& texture : material.mTextureReferences)
    {
        auto handle = request.assets.Load(texture);
        if (!handle)
            return Unexpected(handle.error());

        auto dependency = request.assets.Acquire(*handle);
        info.textures[info.textureCount++] = dependency->GetRuntimeHandle();
        asset->mTextures.push_back(std::move(dependency));
    }
    for (const auto& parameter : asset->mParameters)
        info.parameters[info.parameterCount++] = {parameter.name.c_str(), parameter.value};

    asset->mRuntime = Services::Get<Renderer>().CreateMaterial(info);
    return OwnedPtr<Asset>(std::move(asset));
}

Expected<void, String> MaterialAssetSerializer::Serialize(const Asset& asset, Archive& archive)
{
    if (asset.GetType() != MaterialAsset::TYPE)
        return Unexpected(String("Expected a material asset"));

    const auto& material = static_cast<const MaterialAsset&>(asset);
    if (!archive.WriteString(material.mShaderReference.id.ToString(), "shader") || !archive.BeginWriteArray("textures"))
        return Unexpected(String("Failed to write material shader or textures"));

    bool valid = true;
    for (const auto& texture : material.mTextureReferences)
        valid = archive.WriteString(texture.id.ToString(), "") && valid;
    archive.EndWriteArray();

    if (!valid || !archive.BeginWriteArray("parameters"))
        return Unexpected(String("Failed to write material textures or parameters"));

    for (const auto& parameter : material.mParameters)
    {
        if (!archive.BeginWriteObject(""))
        {
            valid = false;
            break;
        }
        valid = archive.WriteString(parameter.name, "name") && valid;
        if (archive.BeginWriteArray("value"))
        {
            valid = std::visit([&](const auto& value) { return WriteValue(archive, value); }, parameter.value) && valid;
            archive.EndWriteArray();
        }
        else
            valid = false;
        archive.EndWriteObject();
    }
    archive.EndWriteArray();

    if (!valid)
        return Unexpected(String("Failed to write material parameters"));

    return {};
}
