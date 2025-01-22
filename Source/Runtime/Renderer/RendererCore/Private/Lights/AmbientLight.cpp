#include "pch.h"
#include "Lights/AmbientLight.h"

#include "Texture/Texture.h"

AmbientLight::AmbientLight()
	:
	mCubemap(nullptr),
	mSolidColor(Color(1,0,0,1))
{
}

AmbientLight::~AmbientLight()
{

}

void AmbientLight::SetCubemap(TextureCube* aCubemap)
{
	mCubemap = aCubemap;
}

const TextureCube* AmbientLight::GetCubemap() const
{
	return mCubemap;
}

const Color AmbientLight::GetSolidColor() const
{
	return mSolidColor;
}
