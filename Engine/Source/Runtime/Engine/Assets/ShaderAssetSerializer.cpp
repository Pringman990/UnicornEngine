#include "pch.h"
#include "ShaderAssetSerializer.h"
#include "ShaderAsset.h"
#include "AssetFile.h"
#include "Engine/FileSystem.h"

Expected<OwnedPtr<Asset>, String> ShaderAssetSerializer::Deserialize(const AssetLoadRequest& request, Archive& archive)
{
    ShaderAsset data;
    if (!archive.ReadString(data.mVertexSource, "vertexSource") ||
        !archive.ReadString(data.mFragmentSource, "fragmentSource"))
        return Unexpected(String("Shader source paths are missing"));

    if (archive.HasKey("uniforms"))
    {
        if (!archive.BeginReadArray("uniforms"))
            return Unexpected(String("Invalid shader uniform list"));

        bool valid = true;
        const auto count = archive.GetArraySize();
        for (uint32 i = 0; i < count && valid; ++i)
        {
            String uniform;
            valid = archive.ReadString(uniform, "");
            data.mUniforms.push_back(std::move(uniform));
            archive.Next();
        }
        archive.EndReadArray();

        if (!valid)
            return Unexpected(String("Invalid shader uniform name"));
    }

    return Create(data, request);
}

Expected<OwnedPtr<Asset>, String> ShaderAssetSerializer::Create(const Asset& data, const AssetLoadRequest& request)
{
    if (data.GetType() != ShaderAsset::TYPE)
        return Unexpected(String("Expected a shader asset"));

    const auto& shader = static_cast<const ShaderAsset&>(data);
    auto& files = Services::Get<FileSystem>();
    auto vertex = files.ReadAll(AssetSourcePath(request.info.path, shader.mVertexSource));
    if (!vertex)
        return Unexpected(vertex.error().message);
    auto fragment = files.ReadAll(AssetSourcePath(request.info.path, shader.mFragmentSource));
    if (!fragment)
        return Unexpected(fragment.error().message);

    auto asset = MakeOwned<ShaderAsset>();
    asset->mVertexSource = shader.mVertexSource;
    asset->mFragmentSource = shader.mFragmentSource;
    asset->mUniforms = shader.mUniforms;
    auto program = Services::Get<Renderer>().CreateProgram(ShaderSourceCreateInfo{
        std::move(*vertex), std::move(*fragment), asset->mUniforms
    });
    if (!program)
        return Unexpected(program.error());

    asset->mRuntime = *program;
    return OwnedPtr<Asset>(std::move(asset));
}

Expected<void, String> ShaderAssetSerializer::Serialize(const Asset& asset, Archive& archive)
{
    if (asset.GetType() != ShaderAsset::TYPE)
        return Unexpected(String("Expected a shader asset"));

    const auto& shader = static_cast<const ShaderAsset&>(asset);
    if (!archive.WriteString(shader.mVertexSource, "vertexSource") ||
        !archive.WriteString(shader.mFragmentSource, "fragmentSource") || !archive.BeginWriteArray("uniforms"))
        return Unexpected(String("Failed to write shader sources or uniforms"));

    bool valid = true;
    for (const auto& uniform : shader.mUniforms)
        valid = archive.WriteString(uniform, "") && valid;
    archive.EndWriteArray();

    if (!valid)
        return Unexpected(String("Failed to write shader uniforms"));

    return {};
}
