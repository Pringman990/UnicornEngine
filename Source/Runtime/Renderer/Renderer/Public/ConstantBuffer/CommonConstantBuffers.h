//#pragma once
//
//enum ConstantBuffers : uint32
//{
//	eCameraConstantBuffer = 0,
//	eObjectConstantBuffer = 1,
//	eMaterialConstantBuffer = 2,
//	eLightConstantBuffer = 3,
//	eRayConstantBuffer = 4
//};
//
//struct alignas(16) CameraConstantBufferData
//{
//	Matrix worldToClipMatrix = {};
//	Vector3 cameraPosition;
//	uint32 pad;
//};
//
//struct alignas(16) ObjectConstantBufferData
//{
//	Matrix modelToWorld = {};
//	float3 objectMinBounds = {};
//	float pad;
//	float3 objectMaxBounds = {};
//	float pad1;
//
//	bool operator!=(const ObjectConstantBufferData& aOther)
//	{
//		return (aOther.modelToWorld != this->modelToWorld);
//	}
//};
//
//struct alignas(16) MaterialConstantBufferData
//{
//	float4 colors[256];
//};
//
//struct alignas(16) LightConstantBufferData
//{
//	//Matrix temp; //Remove this
//
////	struct alignas(16) PointLight
////	{
////		Vector3 position = {};
////		float radius = 0;
////		Color color = {};
////	}pointLights[MAX_NUMBER_OF_LIGHTS];
////
////	uint32_t numPointLights = 0;
//	//uint32_t pad0 = 0;
//	//int32_t isUsingCubemap = 0;
//	//uint32_t cubemapNumberOfMips = 0;
//	//uint32_t pad1;
//	//Vector4 ambientLightColorAndIntensity = {};
//
//	Vector4 directionalLightDirection = {};
//	Vector4 directionalLightColorAndIntensity = {};
//
//	//bool operator!=(const LightConstantBufferData& aOther)
//	//{
//	//	return (aOther.cubemapNumberOfMips != this->cubemapNumberOfMips) ||
//	//		(aOther.ambientLightColorAndIntensity != this->ambientLightColorAndIntensity) ||
//	//		(aOther.isUsingCubemap != this->isUsingCubemap);
//	//}
//};