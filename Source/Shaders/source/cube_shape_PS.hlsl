#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    result.color = input.color;
    return result;
}