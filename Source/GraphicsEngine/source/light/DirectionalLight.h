#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(const Transform& aTransform, const Color& aColor, float aIntensity);
	~DirectionalLight() = default;

private:

};