#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(const Transform& aTransform, const Color& aColor, float aIntensity, float aRadius);
	~PointLight();

	float GetRadius() const;
	void SetRadius(float aNewRadius);

private:
	float mRadius;
};