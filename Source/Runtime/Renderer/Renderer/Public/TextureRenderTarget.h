#pragma once
#include <Core.h>

class LogicalDevice;

class TextureRenderTarget : public AssetBase<TextureRenderTarget>
{
	friend class TextureFactory;
public:
	struct Metadata
	{
		String name = "NO_NAME";
		Vector2 extent;
	};

	struct ResourceData
	{
		GPUResourceHandle<GPUTexture> gpuHandle;
	};
public:
	TextureRenderTarget();
	~TextureRenderTarget();

	virtual const String& GetName() const override { return mMetadata.name; }

	const TextureRenderTarget::Metadata& GetMetadata() const { return mMetadata; };
private:
	Metadata mMetadata;
	ResourceData mResourceData;
};