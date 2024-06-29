#include "common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType output;
    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    float4 vertexObjectNormal = float4(input.normal, 0.0f);
    float4 vertexWorldNormal = mul(modelToWorld, vertexObjectNormal);
    
    output.position = vertexClipPos;
    output.color = input.color;
    output.uv = input.uv;
    output.normal = vertexWorldNormal;
    return output;
}