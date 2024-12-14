#include "Material.h"

Material::Material()
    :
    mInputLayout(nullptr),
    mMaterialShader(nullptr)
{
}

Material::~Material()
{
}

bool Material::Bind()
{
    mInputLayout->Bind();
    mMaterialShader->Bind();
    return false;
}

const eInputLayoutStandardTypes Material::GetInputLayoutType() const
{
    return mInputLayout->GetType();
}

InputLayout* Material::GetInputLayout()
{
    return mInputLayout;
}
