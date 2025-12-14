#include "RenderAssets/Material.h"

#include "RenderAssets/Texture2D.h"

Material::Material()
	:
	AssetBase({}),
	mIsDirty(true)
{
}

Material::Material(UniqueID128 UUID)
	:
	AssetBase(UUID),
	mIsDirty(true)
{
}

Material::~Material()
{
}

void Material::SetTexture(const String& Param, Texture2D* Texture)
{
	if (!Texture)
	{
		LOG_WARNING("When trying to set material parameter '{}', but the texture was nullptr", Param);
		return;
	}

	if (!DoParamExist(Param))
	{
		LOG_WARNING("Trying to set material parameter '{}' that does not exist", Param);
		return;
	}

	auto& materialParam = mShaderParameters[Param];
	if (materialParam.type != MaterialParameterType::Texture2D)
	{
		LOG_WARNING("Trying to set material parameter '{}', but it's not a Texture2D", Param);
		return;
	}
	materialParam.value = Texture->GetUUID();

	MarkDirty();
}

bool Material::DoParamExist(const String& Name)
{
	return mShaderParameters.find(Name) != mShaderParameters.end();
}
