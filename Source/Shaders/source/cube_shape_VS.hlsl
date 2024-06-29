#include "common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType output;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.color = input.color;
    return output;
}