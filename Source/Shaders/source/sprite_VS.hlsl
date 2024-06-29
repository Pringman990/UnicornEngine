#include "Common.hlsli"

SpritePixelInputType main(SpriteVertexInputType input)
{
    SpritePixelInputType output;
    
    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    output.position = vertexClipPos;
    output.color = input.color;
    output.uv = input.uv;
	
    return output;
}