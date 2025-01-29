#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
Texture2D Texture0 : register(t0);

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;
    result.color = Texture0.Sample(DefaultSampler, input.uv);
    
    return result;
}