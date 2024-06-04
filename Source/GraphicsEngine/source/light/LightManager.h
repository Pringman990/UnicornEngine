#pragma once
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "PointLight.h"

class LightManager
{
public:
	LightManager();
	~LightManager();

	void InitDefault();

	AmbientLight& GetAmbientLight();
	DirectionalLight& GetDirectionalLight();

	void AddPointLight(PointLight aPointLight);
	void ClearPointLights();
	uint32_t GetNumberOfPointLights();
	std::array<PointLight, MAX_NUMBER_OF_LIGHTS>& GetPointLights();

private:
	AmbientLight mAmbientLight;
	DirectionalLight mDirectionalLight;

	std::array<PointLight, MAX_NUMBER_OF_LIGHTS> mPointLights;
	uint32_t mNumPointLights;
};