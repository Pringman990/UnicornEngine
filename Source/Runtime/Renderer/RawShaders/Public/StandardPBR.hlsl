#include "Common.hlsli"

////Vertex////

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

SamplerState DefaultSampler : register(s0);
Texture2D AlbedoTex : register(t0);
Texture2D NormalTex : register(t1);

ColorOutput FSMain(VSOutput input)
{
    ColorOutput result;
    
    ////Just Albedo
    result.color = AlbedoTex.Sample(DefaultSampler, input.uv.xy).rgba + NormalTex.Sample(DefaultSampler, input.uv.xy).rgba;
    
    ////Vertex Normals
    //float3 n = normalize(input.normal) * 0.5f + 0.5f;
    //result.color = float4(n, 1.0f);
    
    ////Light Test
    //float3 albedo = AlbedoTex.Sample(DefaultSampler, input.uv.xy).rgb;
    //float3 n = NormalTex.Sample(DefaultSampler, input.uv.xy).xyz * 2.0f - 1.0f;
    //n = normalize(input.normal);
    //float3 lightDir = normalize(float3(0.5, 0.5, -0.7));
    //float diff = saturate(dot(n, lightDir));
    //result.color = float4(albedo * diff, 1.0f);
    
    return result;
}