#pragma once

enum ConstantBuffers : uint32
{
	eCameraConstantBuffer = 0,
	eObjectConstantBuffer = 1,
	eLightConstantBuffer = 2,
	eRayConstantBuffer = 3
};

struct alignas(16) CameraConstantBufferData
{
	Matrix worldToClipMatrix = {};
	Vector3 cameraPosition;
	uint32 pad;
};

struct alignas(16) ObjectConstantBufferData
{
	Matrix modelToWorld = {};

	bool operator!=(const ObjectConstantBufferData& aOther)
	{
		return (aOther.modelToWorld != this->modelToWorld);
	}
};

struct alignas(16) LightConstantBufferData
{
	//Matrix temp; //Remove this

//	struct alignas(16) PointLight
//	{
//		Vector3 position = {};
//		float radius = 0;
//		Color color = {};
//	}pointLights[MAX_NUMBER_OF_LIGHTS];
//
//	uint32_t numPointLights = 0;
	uint32_t pad0 = 0;
	int32_t isUsingCubemap = 0;
	uint32_t cubemapNumberOfMips = 0;
	uint32_t pad1;
	Vector4 ambientLightColorAndIntensity = {};

//	Vector4 directionalLightDirection = {};
//	Vector4 directionalLightColorAndIntensity = {};

	bool operator!=(const LightConstantBufferData& aOther)
	{
		return (aOther.cubemapNumberOfMips != this->cubemapNumberOfMips) ||
			(aOther.ambientLightColorAndIntensity != this->ambientLightColorAndIntensity) ||
			(aOther.isUsingCubemap != this->isUsingCubemap);
	}
};