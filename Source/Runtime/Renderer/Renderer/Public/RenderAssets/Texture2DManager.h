#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Assets/IAssetLoader.h"
#include "Texture2D.h"

class Textrue2DManager : public IAssetLoader
{
public:
	Textrue2DManager();
	~Textrue2DManager() override;

	virtual Texture2D* Load(const String& VirtualPath) override;
	virtual Texture2D* ImportSource(const String& VirtualSourcePath) override;
	virtual Texture2D* ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;

	virtual bool SupportsExtension(const String& Extension) override
	{
		return IsAnyOf(Extension, "png", "dds", "jpeg");
	}
private:

};