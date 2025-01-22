#pragma once
#include <IAssetManager.h>
#include "Texture.h"

class ITextureFactory;

class TextureResourceManager : public IAssetManager<Texture>
{
	friend class AssetRegistry;
public:
	template<typename T>
	void SetFactory()
	{
		mFactory = new T;
	}

	Texture* LoadTextureFromFile(const std::string& aPath);

	Texture* LoadAsset() override;

	void DestroyTexture(UniqueID aTextureID);

private:
	TextureResourceManager();
	~TextureResourceManager() override;
private:
	ITextureFactory* mFactory;

	//TODO: change to uniqueID
	using Path = std::string;
	std::unordered_map<Path, Texture*> mTextures;
};