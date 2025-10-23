#include "Common.hlsli"

Texture2D Albedo : register(t0);
SamplerState DefaultSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

ColorOutput main(VSOutput input)
{
    ColorOutput output;
    output.color = Albedo.Sample(DefaultSampler, input.uv.xy).rgba;
    
	return output;
}