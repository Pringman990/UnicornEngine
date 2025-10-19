#include "Common.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : TEXCOORD0;
    float4 color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput result;
    
    float4 vertexPos = input.position;
    float4 worldPos = mul(modelToWorld, vertexPos);
    float4 viewPos = mul(viewMatrix, worldPos);
    float4 clipPos = mul(projMatrix, viewPos);
	
    result.position = clipPos;
    result.worldPosition = worldPos;
    result.color = input.color;
   // result.uv = input.uv;
    return result;
}