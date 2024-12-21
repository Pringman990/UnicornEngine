#include "pch.h"
#include "Material.h"

#include "Texture/Texture.h"

Material::Material()
    :
    mInputLayout(nullptr),
    mMaterialShader(nullptr)
{
}

Material::~Material()
{
}

void Material::BindTextures()
{
    for (auto& [slot, texture] : mTextures)
    {
        texture->Bind(slot);
    }
}

const eInputLayoutStandardTypes Material::GetInputLayoutType() const
{
    return mInputLayout->GetType();
}

InputLayout* Material::GetInputLayout()
{
    return mInputLayout;
}

void Material::AddTexture(uint32 aSlot, Texture* aTexture)
{
    mTextures[aSlot] = aTexture;
}
