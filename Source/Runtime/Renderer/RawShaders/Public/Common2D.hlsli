#include "CommonMacros.hlsli"

Texture2D Textures[SRV_HEAP_SIZE] : register(t0);
SamplerState Sampler2D : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    uint textureIndex : TEXTINDEX;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    uint textureIndex : TEXTINDEX;
    float2 uv : TEXCOORD;
};