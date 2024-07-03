#include "GraphicsPch.h"
#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

bool Material::CreateMaterial(const std::string& aColorPath, const std::string& aMaterialPath, const std::string& aNormalPath)
{
	if (!mColorTexture.Create(StringToWString(aColorPath).c_str()))
		return false;

	if (!mNormalTexture.Create(StringToWString(aNormalPath).c_str()))
		return false;

	if (!mMaterialTexture.Create(StringToWString(aMaterialPath).c_str()))
		return false;

	return true;
}

void Material::Bind()
{
	mColorTexture.Bind(1);
	mNormalTexture.Bind(2);
	mMaterialTexture.Bind(3);
}

const Texture* Material::GetTexture(eMaterialTexture aTexture) const
{
	switch (aTexture)
	{
	case eMaterialTexture::eColor:
		return &mColorTexture;
		break;
	case eMaterialTexture::eNormal:
		return &mNormalTexture;
		break;
	case eMaterialTexture::eMaterial:
		return &mMaterialTexture;
		break;
	}
	return nullptr;
}
