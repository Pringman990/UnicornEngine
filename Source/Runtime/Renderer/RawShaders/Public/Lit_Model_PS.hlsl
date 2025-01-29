#include "Common.hlsli"
PixelOutput main(ModelPixelInputType input)
{
    PixelOutput result;
   // result.color = float4(0,1,0,1);
    float4 color = normalize(input.worldPosition);
    result.color = color;
    
    return result;
}