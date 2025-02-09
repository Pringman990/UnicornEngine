#include "Common.hlsli"

MeshPixelInput main(MeshVertexInput input)
{
    MeshPixelInput result;
    
    result.position = input.position;
    result.worldPosition = input.position;
    result.uv = input.uv;
    return result;
}