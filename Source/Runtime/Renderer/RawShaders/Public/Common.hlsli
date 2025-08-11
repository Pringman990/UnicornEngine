#include "CommonAPI.hlsli"

cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float3 cameraPosition;
    uint pad;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelToWorld;
}

struct MeshVertexInput
{
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 color : TEXCOORD0;
    [[vk::location(2)]] float2 uv : TEXCOORD1;
    [[vk::location(3)]] float3 normal : TEXCOORD2;
    [[vk::location(4)]] float3 bitangent : TEXCOORD3;
    [[vk::location(5)]]  float3 tangent : TEXCOORD4;
};

struct MeshPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normal : TEXCOORD2;
    float3 bitangent : TEXCOORD3;
    float3 tangent : TEXCOORD4;
};

struct LineVertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct LinePixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct ColorOutput
{
    float4 color : SV_TARGET0;
};

struct ColorDepthOutput
{
    float4 color : SV_TARGET0;
    float depth : SV_Depth;
};