#pragma once

struct SpriteSheet
{
    List<glm::uvec4> frames;
};

class SpriteSheetConverter
{
public:
    static SpriteSheet Convert(const ByteBuffer& data, uint32 width, uint32 height, uint32 channels);
private:
};