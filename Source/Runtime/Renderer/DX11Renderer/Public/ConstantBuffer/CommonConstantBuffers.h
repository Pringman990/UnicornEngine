#pragma once

enum ConstantBuffers : uint32
{
	eCameraConstantBuffer = 0,
	eObjectConstantBuffer = 1,
	eLightConstantBuffer = 2
};

struct alignas(16) CameraConstantBufferData
{
	Matrix worldToClipMatrix = {};
	Vector3 position = {};
	int32 padding0 = 0;
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
	Matrix temp; //Remove this

//	struct alignas(16) PointLight
//	{
//		Vector3 position = {};
//		float radius = 0;
//		Color color = {};
//	}pointLights[MAX_NUMBER_OF_LIGHTS];
//
//	uint32_t numPointLights = 0;
//	uint32_t pad3 = 0;
//	uint32_t pad4 = 0;
//
//	uint32_t numberOfMips = 0;
//	Vector4 ambientLightColorAndIntensity = {};
//	Vector4 directionalLightDirection = {};
//	Vector4 directionalLightColorAndIntensity = {};
};