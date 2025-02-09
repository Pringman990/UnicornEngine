#include "Common.hlsli"

MeshPixelInput main(MeshVertexInput input)
{
    MeshPixelInput result;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
	
    result.position = vertexClipPos;
    result.worldPosition = vertexWorldPos;
    result.uv = input.uv;
    return result;
}