#pragma once
#include <Core.h>

class Texture2D;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Resize(Vector2 Size);
	void Release(bool ReleaseSRV = true);

	bool IsDepthTesting();

	static RenderTarget* Create(const Vector2& Size, bool EnableDepthTesting = true);
	static RenderTarget* Create(ID3D12Resource* RTVResource, bool EnableDepthTesting);
	static void Create(
		const Vector2& Size,
		RenderTarget* RenderTarget,
		D3D12_RESOURCE_DESC RTVDesc,
		D3D12_RESOURCE_DESC DepthDesc
	);

	ComPtr<ID3D12Resource>& GetResource();
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle();
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle();
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle();

	D3D12_VIEWPORT& GetViewport();
	D3D12_RECT& GetScissorRect();

	Texture2D* GetTexture();

private:
	bool CreateInternal(ID3D12Resource* RTVResource, bool EnableDepthTesting);
private:

	D3D12_CPU_DESCRIPTOR_HANDLE mRTVHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE mDSVHandle;

	D3D12_DEPTH_STENCIL_DESC mDDSDesc;

	ComPtr<ID3D12Resource> mRTV;
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
	bool mIsDepthTesting;
	Texture2D* mSRV;
};