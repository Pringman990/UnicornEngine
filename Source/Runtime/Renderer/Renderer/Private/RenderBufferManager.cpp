#include "RenderBufferManager.h"

#include "Renderer.h"

RenderBufferManager::RenderBufferManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

RenderBufferManager::~RenderBufferManager()
{
}

DirectResourceHandle<GPUBuffer> RenderBufferManager::CreateVertex(const Vector<Vertex>& Verticies)
{
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(Verticies.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = Verticies.data();

	ComPtr<ID3D11Buffer> buffer;
	HRESULT hr = mRenderer->GetLogicalDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, buffer.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create vertex buffer. HRESULT: {}", hr);
		return DirectResourceHandle<GPUBuffer>::Invalid();
	}

	DirectResourceHandle<GPUBuffer> handle = mBufferPool.Allocate();
	handle.ptr->buffer = std::move(buffer);

	return handle;
}

DirectResourceHandle<GPUBuffer> RenderBufferManager::CreateIndex(const Vector<uint32>& Indicies)
{
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(Indicies.size()) * static_cast<UINT>(sizeof(uint32));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = Indicies.data();

	ComPtr<ID3D11Buffer> buffer;
	HRESULT hr = mRenderer->GetLogicalDevice()->CreateBuffer(&indexBufferDesc, &indexSubresourceData, buffer.GetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create index buffer. HRESULT: {}", hr);
		return DirectResourceHandle<GPUBuffer>::Invalid();
	}

	DirectResourceHandle<GPUBuffer> handle = mBufferPool.Allocate();
	handle.ptr->buffer = std::move(buffer);

	return handle;
}

DirectResourceHandle<GPUConstantBuffer> RenderBufferManager::CreateConstantBuffer(uint32 Size, void* InitialData, BufferUsage Usage)
{

	if (Size <= 0)
	{
		LOG_ERROR("Failed to create constant buffer");
		return DirectResourceHandle<GPUConstantBuffer>::Invalid();
	}

	//16 byte aligned
	uint32 alignedSize = (Size + 15) & ~15;
	if (alignedSize != Size)
	{
		LOG_WARNING("Constant buffer padded from {} to {} bytes (must be multiple of 16). Verify shader layout!", Size, alignedSize);
	}

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = ToD11BufferUsage(Usage);
	desc.ByteWidth = alignedSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (Usage == BufferUsage::Dynamic)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else // Default
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}

	ComPtr<ID3D11Buffer> buffer;
	HRESULT result = mRenderer->GetLogicalDevice()->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());
	if (FAILED(result))
	{
		_com_error err(result);
		std::wstring msg = err.ErrorMessage();
		LOG_ERROR("Failed to create constant buffer: {}", WStringToString(msg));

		return DirectResourceHandle<GPUConstantBuffer>::Invalid();
	}

	DirectResourceHandle<GPUConstantBuffer> handle = mConstantBufferPool.Allocate();
	handle.ptr->size = alignedSize;
	handle.ptr->usage = Usage;
	handle.ptr->buffer = std::move(buffer);
	handle.ptr->cpuData.reserve(alignedSize);

	if (InitialData)
	{
		UpdateConstantBuffer(handle, InitialData);
	}

	return handle;
}

void RenderBufferManager::UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data)
{
	if (!Buffer || Buffer.ptr == nullptr || Buffer.ptr->buffer == nullptr || Data == nullptr)
	{
		LOG_ERROR("Failed to update constant buffer");
		return;
	}

	ID3D11DeviceContext* context = mRenderer->GetLogicalDevice().GetImmediateContext();
	if (Buffer.ptr->usage == BufferUsage::Default)
	{
		context->UpdateSubresource(Buffer.ptr->buffer.Get(), 0, nullptr, Data, 0, 0);
	}
	else if (Buffer.ptr->usage == BufferUsage::Dynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(Buffer.ptr->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, Data, Buffer.ptr->size);
		context->Unmap(Buffer.ptr->buffer.Get(), 0);
	}
	else
	{
		LOG_ERROR("Trying to update constant buffer with non implemented update function");
	}

	memcpy_s(Buffer.ptr->cpuData.data(), Buffer.ptr->size, Data, Buffer.ptr->size);
}

void RenderBufferManager::BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStage Stages)
{
	if (!Buffer || Buffer.ptr == nullptr || Buffer.ptr->buffer == nullptr)
	{
		LOG_ERROR("Failed to bind constant buffer");
		return;
	}

	if (HasFlag(Stages, ShaderStage::Undefined))
	{
		LOG_ERROR("Can't bind constant buffer to non implemented shader stage");
	}

	ID3D11DeviceContext* context = mRenderer->GetLogicalDevice().GetImmediateContext();

	if (HasFlag(Stages, ShaderStage::VS))
	{
		context->VSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
	if (HasFlag(Stages, ShaderStage::FS))
	{
		context->PSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
}

void RenderBufferManager::BindConstantBuffers(Vector<DirectResourceHandle<GPUConstantBuffer>> Buffers, uint32 StartSlot, ShaderStage Stages)
{
	if (Buffers.size() == 0)
	{
		LOG_ERROR("Failed to bind constant buffers");
		return;
	}

	ID3D11DeviceContext* context = mRenderer->GetLogicalDevice().GetImmediateContext();

	Vector<ID3D11Buffer*> dxBuffers;
	for (uint32 i = 0; i < Buffers.size(); i++)
	{
		if (Buffers[i].ptr == nullptr)
		{
			LOG_ERROR("Constant buffer is not vaild, removed from binding");
		}

		dxBuffers.push_back(Buffers[i].ptr->buffer.Get());
	}

	if (HasFlag(Stages, ShaderStage::VS))
	{
		context->VSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
	if (HasFlag(Stages, ShaderStage::FS))
	{
		context->PSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
}
