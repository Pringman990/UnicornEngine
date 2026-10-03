#include "pch.h"
#include "MaterialAsset.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"

MaterialAsset::~MaterialAsset()
{
    if (mRuntime)
        Services::Get<Renderer>().DestroyMaterial(mRuntime);
}

void MaterialAsset::SetParameter(const String& name, const MaterialUniformValue& value)
{
    for (auto& parameter : mParameters)
    {
        if (parameter.name == name)
        {
            parameter.value = value;
            return;
        }
    }

    mParameters.push_back({name, value});
}
