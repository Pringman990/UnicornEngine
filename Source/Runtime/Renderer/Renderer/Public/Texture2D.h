#pragma once
#include <Core.h>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	void Resize(const Vector2& NewSize);
	void Release();

	static Texture2D* Create(const std::wstring& DDSPath);
	static Texture2D* Create(const Vector2& Size, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);

	/// <summary>
	///  Used for resizing.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool Create(Texture2D* Texture, const Vector2& NewSize, D3D12_RESOURCE_DESC TextureDesc);
	
	/// <summary>
	/// Used when creating the backbuffer
	/// </summary>
	static Texture2D* Create(ID3D12Resource* RTVResource);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
	ComPtr<ID3D12Resource>& GetResource();

	D3D12_RESOURCE_DESC GetDesc() const;

private:
	ComPtr<ID3D12Resource> mSRV;
	D3D12_CPU_DESCRIPTOR_HANDLE mSRVHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;

	Vector2 mSize;
	uint32 mMipLevel;
};