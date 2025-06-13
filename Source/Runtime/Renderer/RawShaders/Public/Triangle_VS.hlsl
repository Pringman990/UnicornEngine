#include "Common.hlsli"

VSOutput main(uint vertexID : SV_VertexID)
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

    output.Pos = float4(input.position, 0.0f, 1.0f);
    output.Color = input.color;
    return output;
}