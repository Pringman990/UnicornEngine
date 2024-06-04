#include "common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.color = input.color;
    return output;
}