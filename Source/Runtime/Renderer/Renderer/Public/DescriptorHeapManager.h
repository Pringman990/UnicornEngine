#pragma once
#include <Core.h>

#include <d3dx12.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class DescriptorHeapManager
{
public:
	DescriptorHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE aType, uint32 aSize);
	~DescriptorHeapManager();

	bool Init(ID3D12Device* aDevice);
	void Release();

	D3D12_CPU_DESCRIPTOR_HANDLE Allocate();
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE aHandle);

	ID3D12DescriptorHeap* GetHeap();
	D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCPUStart();
	uint32 GetIncrementSize();
private:
	bool Resize();
private:
	ComPtr<ID3D12DescriptorHeap> mHeap;
	D3D12_DESCRIPTOR_HEAP_TYPE mType;
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHeapStart;
	D3D12_DESCRIPTOR_HEAP_FLAGS mFlags;
	uint32 mDescriptorCount;
	uint32 mIncrementSize;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> mFreeList;
	uint32 mNextFreeIndex;
};