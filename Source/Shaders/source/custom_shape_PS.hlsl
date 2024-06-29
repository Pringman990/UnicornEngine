#include "common.hlsli"

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput output;
    output.color = input.color;
    return output;
}