#pragma once
#include <Core.h>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	void Resize(const Vector2& aNewSize);
	void Release();

	static Texture2D* Create(const std::wstring& aDDSPath);
	static Texture2D* Create(const Vector2& aSize, D3D12_RESOURCE_FLAGS someFlags = D3D12_RESOURCE_FLAG_NONE);

	/// <summary>
	///  Used for resizing.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool Create(Texture2D* aTexture, const Vector2& aNewSize, D3D12_RESOURCE_DESC aTextureDesc);
	
	/// <summary>
	/// Used when creating the backbuffer
	/// </summary>
	static Texture2D* Create(ID3D12Resource* aRTVResource);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const;
	ComPtr<ID3D12Resource>& GetResource();

	D3D12_RESOURCE_DESC GetDesc() const;

private:
	ComPtr<ID3D12Resource> mSRV;
	D3D12_CPU_DESCRIPTOR_HANDLE mSRVHandle;

	Vector2 mSize;
	uint32 mMipLevel;
};