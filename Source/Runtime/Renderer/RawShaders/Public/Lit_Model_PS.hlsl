#include "Common.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D texture0 : register(t0);

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;
    result.color = texture0.Sample(defaultSampler, input.uv.xy).rgba;
    
    return result;
}