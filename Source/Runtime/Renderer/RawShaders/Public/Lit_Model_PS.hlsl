#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
Texture2D Texture0 : register(t1);

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;

    if (isUsingCubemap == false)
        result.color = ambientLightColorAndIntensity;
    else
        result.color = Texture0.Sample(DefaultSampler, input.uv.xy).rgba;
    
    return result;
}