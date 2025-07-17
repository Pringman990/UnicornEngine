#include "Common2D.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    return Textures[input.textureIndex].Sample(Sampler2D, input.uv);
}