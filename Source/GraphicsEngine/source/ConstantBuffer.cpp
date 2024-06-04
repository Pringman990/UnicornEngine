#include "GraphicsPch.h"
#include "ConstantBuffer.h"

#include "DX11.h"

ConstantBuffer::ConstantBuffer()
	:
	mSize(0)
{
}

ConstantBuffer::~ConstantBuffer()
{
	mBuffer.Reset();
}

bool ConstantBuffer::Init(uint32_t aSize, void* someData)
{
	if (aSize <= 0 || someData == nullptr)
	{
		std::cout << "Failed to create constant buffer" << std::endl;
		return false;
	}
	
	mSize = aSize;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = aSize;
	HRESULT result = GraphicsEngine::GetInstance().GetDX11().GetDevice()->CreateBuffer(&desc, nullptr, &mBuffer);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create constant buffer: " << errorMessage << std::endl;
		return false;
	}
	return true;
}

void ConstantBuffer::Update(void* someData)
{
	ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetDX11().GetDeviceContext();
	
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, someData, mSize);
	context->Unmap(mBuffer.Get(), 0);
}

void ConstantBuffer::Bind(ConstantBuffers aSlot)
{
	ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetDX11().GetDeviceContext();
	context->VSSetConstantBuffers(aSlot, 1, mBuffer.GetAddressOf());
	context->PSSetConstantBuffers(aSlot, 1, mBuffer.GetAddressOf());
}
