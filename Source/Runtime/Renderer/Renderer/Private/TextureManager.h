#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUTexture.h"

struct ID3D11Texture2D;

class Renderer;

/**
* Manager for GPUTextures and not Texture assets.
*/
class TextureManager
{
public:
	TextureManager(Renderer* InRenderer);
	~TextureManager();

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


	/**
	* @return
	* Pointer to actual GPUTexture from handle.
	*/
	inline GPUTexture* GetInternalTexture(GPUResourceHandle<GPUTexture> Handle)
	{
		return mTexturePool.Get(Handle);
	}

private:
	Renderer* mRenderer;
	ResourcePool<GPUTexture> mTexturePool;
};