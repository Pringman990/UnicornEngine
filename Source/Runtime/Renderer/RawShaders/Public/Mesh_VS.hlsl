#include "Common.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float3 normal : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 bitangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
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
    result.uv = input.uv;
    return result;
}