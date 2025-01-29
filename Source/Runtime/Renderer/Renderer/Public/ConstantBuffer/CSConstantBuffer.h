#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class CSConstantBuffer
{
public:
	CSConstantBuffer();
	~CSConstantBuffer();

	bool Init(const uint32& aSize);
	void UpdateData(void* someData);
	void Bind();

private:
	ComPtr<ID3D11Buffer> mBuffer;
	uint32_t mSize;
};