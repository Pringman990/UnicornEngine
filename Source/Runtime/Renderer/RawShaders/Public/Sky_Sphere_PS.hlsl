#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
TextureCube Texture0 : register(t1);

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;
    
    float3 direction = normalize(input.worldPosition.xyz - cameraPosition);

    float4 skyboxColor = Texture0.SampleLevel(DefaultSampler, direction, 0.0f).rgba;
    result.color = float4(min(skyboxColor.xyz, float3(1.0f, 1.0f, 1.0f)), 1.0f);
    
    return result;
}