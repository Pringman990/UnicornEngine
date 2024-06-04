#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput output;
    output.color = input.color;
    return output;
}