#pragma once
#include "Core/Types.h"

struct TextureLoadData
{
    ByteBuffer data;
    uint32 width;
    uint32 height;
    uint32 channels;
};

class TextureLoader
{
public:

    NODISC static TextureLoadData LoadTexture(const ByteBuffer& file);

private:
};