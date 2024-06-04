#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = colorTexture.Sample(defaultSampler, input.uv.xy).rgba;
    return result;
}