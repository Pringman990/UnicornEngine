#pragma once
#include <IResourceManager.h>
#include "Texture.h"

class ITextureFactory;

class TextureResourceManager : public IResourceManager
{
public:
	TextureResourceManager();
	~TextureResourceManager() override;

	void SetFactory(ITextureFactory* aFactory);

	Texture* LoadTextureFromFile(const std::string& aPath);

	void DestroyTexture(UniqueID aTextureID);

private:

private:
	ITextureFactory* mFactory;

	//TODO: change to uniqueID
	using Path = std::string;
	std::unordered_map<Path, Texture*> mTextures;
};