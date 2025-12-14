#include "Common.hlsli"

struct VSInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float3 normal : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 bitangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

struct VSOutput
{
    float4 position : SV_POSITION0;
    float4 worldPosition : POSITION0;
    float4 color : COLOR0;
    float3 normal : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 bitangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

VSOutput VSMain(VSInput input)
{
    VSOutput result;
    
    float4 vertexPos = input.position;
    float4 worldPos = mul(modelToWorld, vertexPos);
    float4 viewPos = mul(viewMatrix, worldPos);
    float4 clipPos = mul(projMatrix, viewPos);
	
    float4 objectNormal = float4(input.normal, 0.f);
    float4 objectTangent = float4(input.tangent, 0.f);
    float4 objectBitangent = float4(input.bitangent, 0.f);
    float4 worldNormal = mul(modelToWorld, objectNormal);
    float4 worldTangent = mul(modelToWorld, objectTangent);
    float4 worldBitangent = mul(modelToWorld, objectBitangent);
    
    result.position = clipPos;
    result.worldPosition = worldPos;
    result.color = input.color;
    result.normal = worldNormal;
    result.tangent = worldTangent;
    result.bitangent = worldBitangent;
    result.uv = input.uv;
    return result;
}

////Fragment////

struct ColorOutput
{
    float4 color : SV_Target0;
};

ColorOutput FSMain(VSOutput input)
{
    ColorOutput result;
    
    float2 uv = input.uv * 10.0;

    // rotate 45 degrees
    float s = 0.7071; // sin/cos 45°
    float2x2 rot = float2x2(s, -s, s, s);
    uv = mul(uv, rot);

    float stripe = fmod(floor(uv.x), 2);

    float3 colorA = float3(1, 0, 1); // yellow
    float3 colorB = float3(0, 0, 0); // black
    
    result.color = float4(lerp(colorA, colorB, stripe).rgb, 1.0f);
    return result;
}