#include "GraphicsPch.h"
#include "Light.h"

Light::Light(const Transform& aTransform, const Color& aColor, float aIntensity)
	:
	mTransform(aTransform),
	mColor(aColor),
	mIntensity(aIntensity)
{
}

Transform& Light::GetTransform()
{
	return mTransform;
}

float Light::GetIntensity() const
{
	return mIntensity;
}

Color Light::GetColor() const
{
	return mColor;
}

void Light::SetColor(Color aNewColor)
{
	mColor = aNewColor;
}

void Light::SetIntensity(float aNewIntensity)
{
	mIntensity = aNewIntensity;
}
