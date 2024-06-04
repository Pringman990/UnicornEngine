#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;

struct alignas(16) CameraConstantBufferData
{
	Matrix worldToClipMatrix;
	Vector3 position;
	int garbage0;
};

struct alignas(16) ObjectConstantBufferData
{
	Matrix modelToWorld;
};

struct alignas(16) LightConstantBufferData
{
	struct alignas(16) PointLight
	{
		Vector3 position;
		float radius;
		Color color;
	}pointLights[MAX_NUMBER_OF_LIGHTS];
	uint32_t numPointLights;
	uint32_t pad3;
	uint32_t pad4;

	uint32_t numberOfMips;
	Vector4 ambientLightColorAndIntensity;
	Vector4 directionalLightDirection;
	Vector4 directionalLightColorAndIntensity;
};

class ConstantBuffer
{
public:
	enum ConstantBuffers : uint32_t
	{
		eCameraConstantBuffer = 0,
		eObjectConstantBuffer = 1,
		eLightConstantBuffer = 2
	};

public:
	ConstantBuffer();
	~ConstantBuffer();

	bool Init(uint32_t aSize, void* someData);
	void Update(void* someData);
	void Bind(ConstantBuffers aSlot);

private:
	ComPtr<ID3D11Buffer> mBuffer;
	uint32_t mSize;
};