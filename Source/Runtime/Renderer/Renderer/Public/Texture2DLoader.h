#pragma once
#include <AssetManager/AssetLoader.h>
#include "Texture2D.h"

class Texture2DLoader : public AssetLoader<Texture2D>
{
public:
	Texture2DLoader();
	~Texture2DLoader() override;

	virtual AssetLoadResult<Texture2D> Load(const String& VirtualPath, Texture2D* Asset) override;
	virtual bool Save(const String& VirtualPath, AssetHandle<Texture2D> Handle) override;

private:
};