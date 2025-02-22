#include "pch.h"
#include "Texture/TextureResourceManager.h"

#include "Texture/ITextureFactory.h"

TextureResourceManager::TextureResourceManager()
	: 
	mFactory(nullptr)
{
}

TextureResourceManager::~TextureResourceManager()
{
	for (auto it : mTextures)
	{
		delete it.second;
		it.second = nullptr;
	}
	mTextures.clear();
}

Texture* TextureResourceManager::LoadTextureFromFile(const std::string& aPath)
{
	Texture* texture = mFactory->LoadTextureFromFile(aPath);
	mTextures[aPath] = texture;
	return texture;
}

Texture* TextureResourceManager::LoadAsset()
{
	return nullptr;
}

//TODO: fix this again
void TextureResourceManager::DestroyTexture(UniqueID /*aTextureID*/)
{
	//auto it = mTextures.find(aTextureID);
	//if (it != mTextures.end()) {
	//	delete it->second;
	//	mTextures.erase(aTextureID);
	//}
}