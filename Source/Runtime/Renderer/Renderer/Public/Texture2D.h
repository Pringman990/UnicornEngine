#pragma once
#include <Core.h>
#include <AssetManager/AssetBase.h>

#include <GPUTexture.h>

class Texture2D : public AssetBase<Texture2D>
{
	friend class AssetManager;
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

	struct StagingData
	{
		ByteBuffer textureData;
		Vector2 extent;
	};
public:
	Texture2D();
	~Texture2D();

	void FreeStagingData();

	virtual const String& GetName() const override { return mMetadata.name; }
	
	const Texture2D::Metadata& GetMetadata() const { return mMetadata; };
	const Texture2D::ResourceData& GetResourceData() const { return mResourceData; };

	const uint64 GetImGuiHandle() const { return mImGuiHandle; };
	void SetImGuiHandle(uint64 Handle) { mImGuiHandle = Handle; };

private:
	Metadata mMetadata;
	ResourceData mResourceData;
	StagingData mStagingData;

	uint64 mImGuiHandle;
};