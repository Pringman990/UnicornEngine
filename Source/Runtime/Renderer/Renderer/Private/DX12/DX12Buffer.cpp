//#include "Buffer.h"
//
//Buffer::Buffer(Buffer::Type Type, DXGI_FORMAT Format)
//	:
//	mType(Type),
//	mFormat(Format)
//{
//}
//
//Buffer Buffer::CreateVertex(const void* Data, size_t Size)
//{
//	Buffer buffer(Type::Vertex);
//
//	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//	CD3DX12_RESOURCE_DESC vbufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Size);
//
//	HRESULT hr = Renderer::Get()->GetDevice()->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&vbufferDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(buffer.mResource.GetAddressOf())
//	);
//	if (FAILED(hr)) {
//		_LOG_RENDERER_ERROR("Failed to create vertex buffer\n");
//	}
//
//	void* vdata = nullptr;
//	buffer.mResource->Map(0, nullptr, &vdata);
//	memcpy(vdata, Data, Size);
//	buffer.mResource->Unmap(0, nullptr);
//
//	buffer.mVBV.BufferLocation = buffer.mResource->GetGPUVirtualAddress();
//	buffer.mVBV.SizeInBytes = static_cast<UINT>(Size);
//	buffer.mVBV.StrideInBytes = sizeof(Vertex);
//
//	return buffer;
//}
//
//Buffer Buffer::CreateIndex(const void* Data, size_t Size, DXGI_FORMAT Format)
//{
//	Buffer buffer(Type::Index, Format);
//
//	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//	CD3DX12_RESOURCE_DESC ibufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Size);
//
//	HRESULT hr = Renderer::Get()->GetDevice()->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&ibufferDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(buffer.mResource.GetAddressOf())
//	);
//	if (FAILED(hr)) {
//		_LOG_RENDERER_ERROR("Failed to create index buffer\n");
//	}
//
//	void* idata = nullptr;
//	buffer.mResource->Map(0, nullptr, &idata);
//	memcpy(idata, Data, Size);
//	buffer.mResource->Unmap(0, nullptr);
//
//	buffer.mIBV.BufferLocation = buffer.mResource->GetGPUVirtualAddress();
//	buffer.mIBV.SizeInBytes = static_cast<UINT>(Size);;
//	buffer.mIBV.Format = Format;
//
//	return buffer;
//}
//
//Buffer Buffer::CreateConstant(const void* Data, size_t Size, size_t Align)
//{
//	Buffer buffer(Type::Constant);
//
//	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//	CD3DX12_RESOURCE_DESC cbufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Align);
//
//	HRESULT hr = Renderer::Get()->GetDevice()->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&cbufferDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(buffer.mResource.GetAddressOf())
//	);
//	 
//	if (FAILED(hr)) {
//		_LOG_RENDERER_ERROR("Failed to create constant buffer\n");
//	}
//
//	void* idata = nullptr;
//	buffer.mResource->Map(0, nullptr, &idata);
//	memcpy(idata, Data, Size); 
//	buffer.mResource->Unmap(0, nullptr); 
//
//	return buffer; 
//}
// 
//void Buffer::Upload(const void* Data, size_t Size)
//{
//	void* idata = nullptr;
//	mResource->Map(0, nullptr, &idata);
//	memcpy(idata, Data, Size);
//	mResource->Unmap(0, nullptr);
//}
//
//D3D12_VERTEX_BUFFER_VIEW Buffer::GetVBV() const
//{
//	_ENSURE_RENDERER(mType == Type::Vertex, "Buffer type was not correct");
//	return mVBV;
//}
//
//D3D12_INDEX_BUFFER_VIEW Buffer::GetIBV() const
//{
//	_ENSURE_RENDERER(mType == Type::Index, "Buffer type was not correct");
//	return mIBV;
//}
