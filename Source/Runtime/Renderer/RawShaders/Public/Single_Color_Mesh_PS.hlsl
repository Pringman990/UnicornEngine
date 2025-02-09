#include "Common.hlsli"

ColorOutput main(MeshPixelInput input)
{
    ColorOutput result;
    result.color =float4(1.0f, 0.0f, 0.0f, 1.0f);
    //result.depth = input.position.z / input.position.w;
    return result;
}