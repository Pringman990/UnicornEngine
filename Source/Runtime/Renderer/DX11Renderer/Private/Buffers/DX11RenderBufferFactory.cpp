#include "pch.h"
#include "DX11RenderBufferFactory.h"

#include "DX11Buffers.h"

DX11RenderBufferFactory::DX11RenderBufferFactory()
{
}

DX11RenderBufferFactory::~DX11RenderBufferFactory()
{
}

RenderBuffer* DX11RenderBufferFactory::CreateVertexBuffer(const std::vector<Vertex>& aVertexArray)
{
    DX11VertexBuffer* buffer = new DX11VertexBuffer();
	
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(aVertexArray.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &aVertexArray[0];

	ID3D11Device* device =  DX11RenderingBackend::_GetInstance()->GetDevice();
	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &buffer->mVertexBuffer);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create vertex buffer. HRESULT: {}", hr);
		return nullptr;
	}

    return buffer;
}

RenderBuffer* DX11RenderBufferFactory::CreateIndexBuffer(const std::vector<uint32>& aIndexArray)
{
	DX11IndexBuffer* buffer = new DX11IndexBuffer();

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(aIndexArray.size()) * static_cast<UINT>(sizeof(uint32));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &aIndexArray[0];

	ID3D11Device* device = DX11RenderingBackend::_GetInstance()->GetDevice();
	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &buffer->mIndexBuffer);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create index buffer. HRESULT: {}", hr);
		return nullptr;
	}
    return buffer;
}
