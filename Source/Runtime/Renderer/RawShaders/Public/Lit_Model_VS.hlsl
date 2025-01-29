#include "Common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType result;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
	
    result.position = vertexClipPos;
    result.worldPosition = input.position;
    result.uv = input.uv;
    
    return result;
}