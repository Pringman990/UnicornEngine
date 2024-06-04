#include "GraphicsPch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const Transform& aTransform, const Color& aColor, float aIntensity) : Light(aTransform, aColor, aIntensity)
{
}
