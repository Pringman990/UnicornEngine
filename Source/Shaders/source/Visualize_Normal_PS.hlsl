#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput output;
    
    output.color = float4(input.normal.xyz, 1.0f);
   // output.color = float4(1,1,1,1);
    return output;
}