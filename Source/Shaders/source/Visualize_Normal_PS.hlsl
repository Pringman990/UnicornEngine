#include "common.hlsli"

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput output;
    
    output.color = float4(input.normal.xyz, 1.0f);
   // output.color = float4(1,1,1,1);
    return output;
}