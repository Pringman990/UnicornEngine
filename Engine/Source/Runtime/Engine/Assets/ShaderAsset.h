#pragma once

#include "Asset.h"
#include "Renderer/Renderer.h"

class ShaderAssetSerializer;

class ShaderAsset final : public Asset
{
    INIT_ASSET("Shader")

public:
    ~ShaderAsset() override;

    const String& GetVertexSource() const { return mVertexSource; }
    const String& GetFragmentSource() const { return mFragmentSource; }
    const List<String>& GetUniforms() const { return mUniforms; }
    void SetVertexSource(String source) { mVertexSource = std::move(source); }
    void SetFragmentSource(String source) { mFragmentSource = std::move(source); }
    void SetUniforms(List<String> uniforms) { mUniforms = std::move(uniforms); }
    ShaderProgramHandle GetRuntimeHandle() const { return mRuntime; }

private:
    friend class ShaderAssetSerializer;

    String mVertexSource;
    String mFragmentSource;
    List<String> mUniforms;
    ShaderProgramHandle mRuntime{};
};
