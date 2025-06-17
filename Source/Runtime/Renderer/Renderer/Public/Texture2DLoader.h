#pragma once
#include <AssetManager/AssetLoader.h>
#include "Texture2D.h"

class Texture2DLoader : public AssetLoader<Texture2D>
{
public:
	Texture2DLoader();
	~Texture2DLoader() override;

	virtual Optional<Texture2D> Load(const String& VirtualPath) override;
	virtual bool Save(const String& VirtualPath, SharedPtr<Texture2D> Data) override;

private:
};