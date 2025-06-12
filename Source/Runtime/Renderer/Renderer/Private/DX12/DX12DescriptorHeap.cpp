//#include "pch.h"
//#include "DescriptorHeap.h"
//
//DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32 DescriptorCount)
//	:
//	mHeap(nullptr),
//	mType(Type),
//	mCPUHeapStart(),
//	mFlags(D3D12_DESCRIPTOR_HEAP_FLAG_NONE),
//	mDescriptorCount(DescriptorCount),
//	mIncrementSize(0),
//	mNextFreeIndex(0)
//{
//
//}
//
//DescriptorHeap::~DescriptorHeap()
//{
//	Release();
//}
//
//void DescriptorHeap::Release()
//{
//	mHeap.Reset();
//	mFreeList.clear();
//}
//
//bool DescriptorHeap::Init(ID3D12Device* Device)
//{
//	mFlags = (mType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || mType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ?
//		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//	desc.NumDescriptors = mDescriptorCount;
//	desc.Type = mType;
//	desc.Flags = mFlags;
//	desc.NodeMask = 0;
//
//	HRESULT hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mHeap.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to init DescriptorHeap: {}", hr);
//		return false;
//	}
//	
//	mIncrementSize = Device->GetDescriptorHandleIncrementSize(mType);
//	mCPUHeapStart = mHeap->GetCPUDescriptorHandleForHeapStart();
//	mGPUHeapStart = mHeap->GetGPUDescriptorHandleForHeapStart();
//
//	return true;
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate()
//{
//	if (mNextFreeIndex >= mDescriptorCount)
//	{
//		_LOG_RENDERER_INFO("No more space in DescriptorHeap, Resizing");
//		if (!Resize())
//		{
//			_LOG_RENDERER_ERROR("Failed to resize DescriptorHeap");
//			return D3D12_CPU_DESCRIPTOR_HANDLE();
//		}
//	}
//
//	D3D12_CPU_DESCRIPTOR_HANDLE handle;
//	if (mFreeList.empty())
//	{
//		handle.ptr = mCPUHeapStart.ptr + (mNextFreeIndex * mIncrementSize);
//		mNextFreeIndex++;
//	}
//	else
//	{
//		handle = mFreeList.back();
//		mFreeList.pop_back();
//	}
//
//	return handle;
//}
//
//void DescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
//{
//	if (Handle.ptr < mCPUHeapStart.ptr ||
//		Handle.ptr >= (mCPUHeapStart.ptr + (mDescriptorCount * mIncrementSize)))
//	{
//		_LOG_RENDERER_WARNING("Tried to free an invalid descriptor handle!");
//		return;
//	}
//
//	if (std::find_if(mFreeList.begin(), mFreeList.end(), 
//		[&](const D3D12_CPU_DESCRIPTOR_HANDLE& handle) { return handle.ptr == Handle.ptr; }) == mFreeList.end())
//	{
//		mFreeList.push_back(Handle);
//	}
//}
//
//bool DescriptorHeap::Resize()
//{
//	uint32 newSize = mDescriptorCount + (mDescriptorCount / 2);
//
//	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//	desc.NumDescriptors = newSize;
//	desc.Type = mType;
//	desc.Flags = mFlags;
//	desc.NodeMask = 0;
//
//	ID3D12Device* device = Renderer::Get()->GetDevice();
//
//	ComPtr<ID3D12DescriptorHeap> newHeap;
//	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(newHeap.GetAddressOf()));
//	if (FAILED(hr))
//	{
//		_LOG_RENDERER_CRITICAL("Failed to resize DescriptorHeap: {}", hr);
//		return false;
//	}
//
//	D3D12_CPU_DESCRIPTOR_HANDLE newHeapStart = newHeap->GetCPUDescriptorHandleForHeapStart();
//
//	device->CopyDescriptorsSimple(mNextFreeIndex, newHeapStart, mCPUHeapStart, mType);
//
//	mHeap = newHeap;
//	mCPUHeapStart = newHeapStart;
//	mDescriptorCount = newSize;
//	
//	return true;
//}
//
//ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
//{
//	return mHeap.Get();
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHeapCPUStart()
//{
//	return mCPUHeapStart;
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandleFromCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle)
//{
//	size_t offset = (CpuHandle.ptr - mCPUHeapStart.ptr) / mIncrementSize;
//	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = mGPUHeapStart;
//	gpuHandle.ptr += offset * mIncrementSize;
//	return gpuHandle;
//}
//
//uint32 DescriptorHeap::GetIncrementSize()
//{
//	return mIncrementSize;
//}