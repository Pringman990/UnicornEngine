#include "Common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType result;
    
    result.position = input.position;
    result.uv = input.uv;
    return result;
}