#include "pch.h"
#include "Texture/Texture.h"

#include "Texture/TextureResourceManager.h"
#include <AssetRegistry.h>

Texture::Texture()
	:
	mMipLevel(0),
	mTextureID(UniqueID::InvalidID),
	mSize(0,0),
	mType(TextureType::eInvalid)
{
}

Texture::~Texture()
{
}

void Texture::Destroy()
{
	TextureResourceManager* textureManager = AssetRegistry::GetInstance()->GetManager<TextureResourceManager>();
	textureManager->DestroyTexture(mTextureID);
}

const TextureType Texture::GetTextureType() const
{
	return mType;
}
