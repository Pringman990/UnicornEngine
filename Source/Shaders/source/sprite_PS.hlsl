#include "common.hlsli"

PixelOutput main(SpritePixelInputType input)
{
    PixelOutput output;
    output.color = colorTexture.Sample(defaultSampler, input.uv) * input.color;   
    return output;
}