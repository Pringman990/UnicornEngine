#include "GraphicsPch.h"
#include "PointLight.h"

PointLight::PointLight() : Light({},{},{}), mRadius(1.0f)
{
}

PointLight::PointLight(const Transform& aTransform, const Color& aColor, float aIntensity, float aRadius)
	:
	Light(aTransform, aColor, aIntensity),
	mRadius(aRadius)
{
}

PointLight::~PointLight()
{
}

float PointLight::GetRadius() const
{
	return mRadius;
}

void PointLight::SetRadius(float aNewRadius)
{
	mRadius = aNewRadius;
}
