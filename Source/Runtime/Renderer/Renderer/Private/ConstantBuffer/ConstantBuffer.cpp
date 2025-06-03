//#include "pch.h"
//#include "ConstantBuffer/ConstantBuffer.h"
//
//ConstantBuffer::ConstantBuffer()
//	:
//	mSize(0)
//{
//}
//
//ConstantBuffer::~ConstantBuffer()
//{
//	mBuffer.Reset();
//}
//
//bool ConstantBuffer::Init(uint32_t aSize, void* someData)
//{
//	if (aSize <= 0 || someData == nullptr)
//	{
//		_LOG_RENDERER_ERROR("Failed to create constant buffer");
//		return false;
//	}
//
//	mSize = aSize;
//
//	D3D11_BUFFER_DESC desc = {};
//	desc.Usage = D3D11_USAGE_DYNAMIC;
//	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	desc.ByteWidth = aSize;
//
//	D3D11_SUBRESOURCE_DATA initData;
//	initData.pSysMem = someData;
//	initData.SysMemPitch = aSize;
//
//	HRESULT result = Renderer::Get()->GetDevice()->CreateBuffer(&desc, &initData, &mBuffer);
//	if (FAILED(result))
//	{
//		_com_error err(result);
//		std::wstring msg = err.ErrorMessage();
//		_LOG_RENDERER_ERROR("Failed to create constant buffer: {}", WStringToString(msg));
//
//		return false;
//	}
//	return true;
//}
//
//void ConstantBuffer::UpdateData(void* someData)
//{
//	ID3D11DeviceContext* context = Renderer::Get()->GetDeviceContext();
//
//	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
//	context->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
//	memcpy(mappedBuffer.pData, someData, mSize);
//	context->Unmap(mBuffer.Get(), 0);
//}
//
//void ConstantBuffer::Bind(ConstantBuffers aSlot)
//{
//	ID3D11DeviceContext* context = context = Renderer::Get()->GetDeviceContext();
//
//	context->VSSetConstantBuffers(aSlot, 1, mBuffer.GetAddressOf());
//	context->PSSetConstantBuffers(aSlot, 1, mBuffer.GetAddressOf());
//	context->CSSetConstantBuffers(aSlot, 1, mBuffer.GetAddressOf());
//}
