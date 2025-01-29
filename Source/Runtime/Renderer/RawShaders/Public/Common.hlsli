
cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    uint pad;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelToWorld;
}

struct ModelVertexInputType
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct ModelPixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD0;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};