#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"
#include <Assets/AssetBase.h>

#include "GPUResources/GPUTexture.h"

class Texture2D : public AssetBase
{
public:
	Texture2D();
	Texture2D(UniqueID128 UUID);
	~Texture2D() override;


	void SetGPUTextureHandle(GPUResourceHandle<GPUTexture> Handle) { mGPUTexture = Handle; };
	GPUResourceHandle<GPUTexture> GetGPUTextureHandle() const { return mGPUTexture; };

private:
	GPUResourceHandle<GPUTexture> mGPUTexture;
};