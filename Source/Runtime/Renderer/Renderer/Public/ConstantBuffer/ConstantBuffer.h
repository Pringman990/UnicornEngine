//#pragma once
//#include "ConstantBuffer/CommonConstantBuffers.h"
//
//#include <d3d11.h>
//#include <wrl/client.h>
//using Microsoft::WRL::ComPtr;
//
//class ConstantBuffer
//{
//public:
//	ConstantBuffer();
//	~ConstantBuffer();
//
//	bool Init(uint32_t aSize, void* someData);
//	void UpdateData(void* someData);
//	void Bind(ConstantBuffers aSlot);
//
//private:
//	ComPtr<ID3D11Buffer> mBuffer;
//	uint32_t mSize;
//};