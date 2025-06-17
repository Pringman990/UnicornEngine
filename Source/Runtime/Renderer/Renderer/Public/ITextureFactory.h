#pragma once
#include "Texture2D.h"

class ITextureFactory
{
public:
	ITextureFactory() = default;
	~ITextureFactory() = default;

	virtual Optional<Texture2D> CreateTexture2D(const ByteBuffer& Buffer, int32 Width, int32 Height) = 0;
};