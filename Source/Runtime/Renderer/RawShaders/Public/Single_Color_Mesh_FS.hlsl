#include "Common.hlsli"

cbuffer ConstantTest : register(b1)
{
    float time;
    float3 _padding1;
};

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
    result.color = float4(1,0,0,1);
    return result;
}