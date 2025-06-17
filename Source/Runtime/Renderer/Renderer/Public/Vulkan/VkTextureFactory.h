#pragma once
#include "ITextureFactory.h"

class VkTextureFactory : public ITextureFactory
{
public:

	virtual Optional<Texture2D> CreateTexture2D(const ByteBuffer& Buffer, int32 Width, int32 Height) override;

};