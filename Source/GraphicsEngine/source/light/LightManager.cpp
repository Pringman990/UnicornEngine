#include "GraphicsPch.h"
#include "LightManager.h"

LightManager::LightManager()
    :
    mAmbientLight({1,1,1,1}, 1.0f),
    mDirectionalLight({}, {1.0f, 0.5f, 0.5f, 1}, 1.0f),
    mNumPointLights(0)
{
}

LightManager::~LightManager()
{
}

void LightManager::InitDefault()
{
    mAmbientLight.SetCubeMap(L"../EngineAssets/Cubemap/cube_1024_preblurred_angle3_Skansen3.dds");
    mDirectionalLight.GetTransform().SetRotation({1,0,0});
}

AmbientLight& LightManager::GetAmbientLight()
{
    return mAmbientLight;
}

DirectionalLight& LightManager::GetDirectionalLight()
{
    return mDirectionalLight;
}

void LightManager::AddPointLight(PointLight aPointLight)
{
    assert(mNumPointLights < MAX_NUMBER_OF_LIGHTS);

    if (mNumPointLights >= MAX_NUMBER_OF_LIGHTS)
        return;

    mPointLights[mNumPointLights] = aPointLight;
    mNumPointLights++;
}

void LightManager::ClearPointLights()
{
    mNumPointLights = 0;
}

uint32_t LightManager::GetNumberOfPointLights()
{
    return mNumPointLights;
}

std::array<PointLight, MAX_NUMBER_OF_LIGHTS>& LightManager::GetPointLights()
{
    return mPointLights;
}
