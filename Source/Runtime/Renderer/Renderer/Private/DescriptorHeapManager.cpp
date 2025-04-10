#include "pch.h"
#include "DescriptorHeapManager.h"

DescriptorHeapManager::DescriptorHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE aType, uint32 aDescriptorCount)
	:
	mHeap(nullptr),
	mType(aType),
	mCPUHeapStart(),
	mFlags(D3D12_DESCRIPTOR_HEAP_FLAG_NONE),
	mDescriptorCount(aDescriptorCount),
	mIncrementSize(0),
	mNextFreeIndex(0)
{

}

DescriptorHeapManager::~DescriptorHeapManager()
{
	Release();
}

void DescriptorHeapManager::Release()
{
	mHeap.Reset();
	mFreeList.clear();
}

bool DescriptorHeapManager::Init(ID3D12Device* aDevice)
{
	mFlags = (mType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || mType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ?
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = mDescriptorCount;
	desc.Type = mType;
	desc.Flags = mFlags;
	desc.NodeMask = 0;

	HRESULT hr = aDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		_LOG_RENDERER_CRITICAL("Failed to init DescriptorHeapManager: {}", hr);
		return false;
	}
	
	mIncrementSize = aDevice->GetDescriptorHandleIncrementSize(mType);
	mCPUHeapStart = mHeap->GetCPUDescriptorHandleForHeapStart();

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::Allocate()
{
	if (mNextFreeIndex >= mDescriptorCount)
	{
		_LOG_RENDERER_INFO("No more space in DescriptorHeap, Resizing");
		if (!Resize())
		{
			_LOG_RENDERER_ERROR("Failed to resize DescriptorHeapManager");
			return D3D12_CPU_DESCRIPTOR_HANDLE();
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	if (mFreeList.empty())
	{
		handle.ptr = mCPUHeapStart.ptr + (mNextFreeIndex * mIncrementSize);
		mNextFreeIndex++;
	}
	else
	{
		handle = mFreeList.back();
		mFreeList.pop_back();
	}

	return handle;
}

void DescriptorHeapManager::Free(D3D12_CPU_DESCRIPTOR_HANDLE aHandle)
{
	if (aHandle.ptr < mCPUHeapStart.ptr ||
		aHandle.ptr >= (mCPUHeapStart.ptr + (mDescriptorCount * mIncrementSize)))
	{
		_LOG_RENDERER_WARNING("Tried to free an invalid descriptor handle!");
		return;
	}

	if (std::find_if(mFreeList.begin(), mFreeList.end(), 
		[&](const D3D12_CPU_DESCRIPTOR_HANDLE& handle) { return handle.ptr == aHandle.ptr; }) == mFreeList.end())
	{
		mFreeList.push_back(aHandle);
	}
}

bool DescriptorHeapManager::Resize()
{
	uint32 newSize = mDescriptorCount + (mDescriptorCount / 2);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = newSize;
	desc.Type = mType;
	desc.Flags = mFlags;
	desc.NodeMask = 0;

	ID3D12Device* device = Renderer::GetInstance()->GetDevice();

	ComPtr<ID3D12DescriptorHeap> newHeap;
	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(newHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		_LOG_RENDERER_CRITICAL("Failed to resize DescriptorHeapManager: {}", hr);
		return false;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE newHeapStart = newHeap->GetCPUDescriptorHandleForHeapStart();

	device->CopyDescriptorsSimple(mNextFreeIndex, newHeapStart, mCPUHeapStart, mType);

	mHeap = newHeap;
	mCPUHeapStart = newHeapStart;
	mDescriptorCount = newSize;
	
	return true;
}

ID3D12DescriptorHeap* DescriptorHeapManager::GetHeap()
{
	return mHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetHeapCPUStart()
{
	return mCPUHeapStart;
}

uint32 DescriptorHeapManager::GetIncrementSize()
{
	return mIncrementSize;
}