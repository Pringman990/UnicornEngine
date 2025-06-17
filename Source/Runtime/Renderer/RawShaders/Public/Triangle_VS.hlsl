#include "Common.hlsli"

BIND(0,0)
cbuffer UniformBufferObject : register(b0)
{
    float4x4 model;
    float4x4 worldToClip;
};

struct VSInput
{
    LOCATION(0)  float2 position : POSITION0;
    LOCATION(1)  float4 color : COLOR0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    float4 vertexObjectPos = float4(input.position.xy, 0.0f, 1.0f);
    float4 vertexWorldPos = mul(model, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClip, vertexWorldPos);
    output.Pos = vertexClipPos;
    output.Color = input.color;
    return output;
}