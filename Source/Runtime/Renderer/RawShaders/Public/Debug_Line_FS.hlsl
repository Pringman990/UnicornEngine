#include "Common.hlsli"

ColorOutput main(LinePixelInput input)
{
    ColorOutput output;
    output.color = input.color;
    return output;
}