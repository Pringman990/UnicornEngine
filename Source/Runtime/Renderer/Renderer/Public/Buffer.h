#pragma once
#include <Core.h>

class Buffer
{
public:
	enum class Type
	{
		Vertex,
		Index,
		Constant,
		Structured
	};


	static Buffer CreateVertex(const void* Data, size_t Size);
	static Buffer CreateIndex(const void* Data, size_t Size, DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN);
	static Buffer CreateConstant(const void* Data, size_t Size, size_t Align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	void Upload(const void* Data, size_t Size);

	D3D12_VERTEX_BUFFER_VIEW GetVBV() const;
	D3D12_INDEX_BUFFER_VIEW GetIBV() const;
	 
private:
	Buffer(Buffer::Type Type, DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN); 
private:
	ComPtr<ID3D12Resource> mResource;  
	const Type mType;  

	D3D12_VERTEX_BUFFER_VIEW mVBV;
	D3D12_INDEX_BUFFER_VIEW mIBV;
	DXGI_FORMAT mFormat;
};