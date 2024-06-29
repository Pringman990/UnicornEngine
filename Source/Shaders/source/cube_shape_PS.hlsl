#include "common.hlsli"

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    return result;
}