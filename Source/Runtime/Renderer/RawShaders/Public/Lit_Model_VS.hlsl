#include "Common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType result;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);

    float3x3 toWorldRotation = (float3x3) modelToWorld;
    float3 vertexWorldNormal = normalize(mul(toWorldRotation, input.normal));
    float3 vertexWorldBinormal = mul(toWorldRotation, input.bitangent);
    float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);
	
    result.position = vertexClipPos;
    result.worldPosition = vertexWorldPos;
    result.color = input.color;
    result.uv = input.uv;
    result.normal = vertexWorldNormal;
    result.bitangent = vertexWorldBinormal;
    result.tangent = vertexWorldTangent;
    
    return result;
}