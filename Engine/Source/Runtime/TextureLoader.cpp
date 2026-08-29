//
// Created on 2026-08-15.
//

#include "pch.h"
#include "TextureLoader.h"

#include <stb_image.h>

TextureLoadData TextureLoader::LoadTexture(const ByteBuffer& file)
{
    int width, height, channels = 0;
    unsigned char* data = stbi_load_from_memory(file.data(), file.size(), &width, &height, &channels, 0);
    if (data == nullptr)
    {
        LOG_WARNING("Failed to load image: ", stbi_failure_reason());
        return {};
    }

    TextureLoadData result{};
    result.width = width;
    result.height = height;
    result.channels = channels;

    uint32 textureSize = width * height * result.channels;
    result.data.resize(textureSize);
    memcpy_s(result.data.data(), result.data.size(), data, textureSize);

    stbi_image_free(data);

    return result;
}
