
cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    uint pad;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelToWorld;
    float3 objectMinBounds;
    float pad2;
    float3 objectMaxBounds;
    float pad1;
}

cbuffer MaterialConstantBuffer : register(b2)
{
    float4 colors[256];
}

cbuffer LightConstantBuffer : register(b3)
{
    float4 directionalLightDirection;
    float4 directionalLightColorAndIntensity;
}

struct MeshVertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0; 
};

struct MeshPixelInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : TEXCOORD0;
    float2 uv : TEXCOORD1;
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