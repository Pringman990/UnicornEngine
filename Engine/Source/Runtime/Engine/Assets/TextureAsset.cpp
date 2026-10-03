#include "pch.h"
#include "TextureAsset.h"

TextureAsset::~TextureAsset()
{
    if (mRuntime)
        Services::Get<Renderer>().DestroyTexture(mRuntime);
}
