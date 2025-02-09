#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
Texture2D Texture0 : register(t0);
Texture2D colorTexture : register(t1);
Texture2D worldPositionTexture : register(t2);

ColorOutput main(MeshPixelInput input)
{
    ColorOutput result;
   // result.color = Texture0.Sample(DefaultSampler, input.uv);
 //   result.color = Texture0.Sample(DefaultSampler, input.uv);
    result.color = colorTexture.Sample(DefaultSampler, input.uv);
    //result.depth = worldPositionTexture.Sample(DefaultSampler, input.uv).z;
   // result.color = float4(1,0,0,1);
    return result;
}