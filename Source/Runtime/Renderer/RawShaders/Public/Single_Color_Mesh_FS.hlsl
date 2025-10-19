#include "Common.hlsli"

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : TEXCOORD0;
    float4 color : COLOR;
};

ColorOutput main(VSOutput input)
{
    ColorOutput result;
    //result.color = input.color;
    result.color = input.color * input.worldPosition;
    return result;
}