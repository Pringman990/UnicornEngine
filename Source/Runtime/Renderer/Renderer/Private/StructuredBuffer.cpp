#include "pch.h"
#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer()
{
}

StructuredBuffer::~StructuredBuffer()
{
}

StructuredBuffer* StructuredBuffer::Create(uint32 aTypeSize, uint32 aDataSize, void* someInitalData)
{
	StructuredBuffer* buffer = new StructuredBuffer();
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = aTypeSize * aDataSize;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = aTypeSize;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = someInitalData;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, buffer->mStructuredBuffer.GetAddressOf());
	if (FAILED(hr)) 
	{
		_LOG_CORE_ERROR("Failed to create structured buffer: {}", hr);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = aDataSize;

	hr = device->CreateShaderResourceView(buffer->mStructuredBuffer.Get(), &srvDesc, buffer->mSRV.GetAddressOf());
	if (FAILED(hr)) 
	{
		_LOG_CORE_ERROR("Failed to create structured buffer srv: {}", hr);
	}

	return buffer;
}

void StructuredBuffer::Bind(uint32 aSlot)
{
	Renderer::GetInstance()->GetDeviceContext()->CSSetShaderResources(aSlot, 1, mSRV.GetAddressOf());
}
