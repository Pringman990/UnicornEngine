#include "pch.h"
#include "Texture/Texture.h"

#include "Texture/TextureResourceManager.h"
#include <ResourceRegistry.h>

Texture::Texture()
	:
	mType(eTextureType::eInvalid),
	mMipLevel(0),
	mTextureID(UniqueID::InvalidID),
	mSize(0,0)
{
}

Texture::~Texture()
{
}

void Texture::Destroy()
{
	TextureResourceManager* textureManager = ResourceRegistry::GetInstance()->GetManager<TextureResourceManager>();
	textureManager->DestroyTexture(mTextureID);
}

eTextureType Texture::GetType()
{
	return mType;
}
