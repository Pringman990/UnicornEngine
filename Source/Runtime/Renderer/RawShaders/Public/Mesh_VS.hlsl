#include "Common.hlsli"

MeshPixelInput main(MeshVertexInput input)
{
    MeshPixelInput result;
    
    float4 vertexPos = input.position;
    float4 worldPos = mul(modelToWorld, vertexPos);
    float4 viewPos = mul(viewMatrix, worldPos);
    float4 clipPos = mul(projMatrix, viewPos);
	
    result.position = clipPos;
    result.worldPosition = worldPos;
    result.uv = input.uv;
    return result;
}