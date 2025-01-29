#include "pch.h"
#include "ConstantBuffer/CSConstantBuffer.h"

CSConstantBuffer::CSConstantBuffer()
	:
	mBuffer(nullptr),
	mSize(0)
{
}

CSConstantBuffer::~CSConstantBuffer()
{
	mBuffer.Reset();
}

bool CSConstantBuffer::Init(const uint32& aSize)
{
	D3D11_BUFFER_DESC desc = { };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = aSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&desc, nullptr, mBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to init CSConstantBuffer");
		return false;
	}
	mSize = aSize;
	return true;
}

void CSConstantBuffer::UpdateData(void* someData)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, someData, mSize);
	context->Unmap(mBuffer.Get(), 0);
}

void CSConstantBuffer::Bind()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->CSSetConstantBuffers(0,1,mBuffer.GetAddressOf());
}
