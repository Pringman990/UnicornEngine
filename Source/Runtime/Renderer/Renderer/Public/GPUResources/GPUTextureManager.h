#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUResources/GPUTexture.h"

struct ID3D11Texture2D;

class Renderer;

struct TextureReadContext
{
	const void* data;
	uint32 rowPitch;
	uint32 width;
	uint32 height;
	RenderFormat format;
};

using TextureReadCallback = Func<void(const TextureReadContext& context)>;

/**
* Manager for GPUTextures and not Texture assets.
*/
class GPUTextureManager
{
public:
	GPUTextureManager(Renderer* InRenderer);
	~GPUTextureManager();

	GPUResourceHandle<GPUTexture> CreateTexture2D(const Vector2i& Extent, RenderFormat Format, TextureBindFlags Bind, TextureUsage Usage = TextureUsage::Undefined);

	/**
	* @warning
	* Only 2D textures are supported for now.
	*/
	GPUResourceHandle<GPUTexture> CreateTexture(const ByteBuffer& Buffer, const Vector3i& Extent, RenderFormat Format, TextureBindFlags Bind, TextureUsage Usage = TextureUsage::Undefined);

	/**
	* Used to create a rendertarget for the swapchain so it can be writen to in shaders.
	*/
	GPUResourceHandle<GPUTexture> CreateTextureRenderTarget(ID3D11Texture2D* Texture);


	/**
	* Used for the swapchain to make a srv into the texture2d.
	*/
	void CreateTextureView(GPUTexture* Texture);


	void ResizeTexture2D(GPUResourceHandle<GPUTexture> Handle, const Vector2i& NewExtent);

	/**
	* @return
	* Pointer to actual GPUTexture from handle.
	*/
	inline GPUTexture* GetInternalTexture(GPUResourceHandle<GPUTexture> Handle)
	{
		return mTexturePool.Get(Handle);
	}

	inline void FreeTexture(GPUResourceHandle<GPUTexture> Handle)
	{
		return mTexturePool.Remove(Handle);
	}

	Vector<uint32> GetTextureAsArray(GPUResourceHandle<GPUTexture> Handle);

	void ReadTexture(GPUResourceHandle<GPUTexture> Handle, TextureReadCallback Callback);

private:
	Renderer* mRenderer;
	ResourcePool<GPUTexture> mTexturePool;
};
