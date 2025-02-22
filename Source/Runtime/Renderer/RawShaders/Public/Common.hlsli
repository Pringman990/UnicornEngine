
TextureCube Cubemap : register(t0);

cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    int padding0;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelToWorld;
}

cbuffer LightConstantBuffer : register(b2)
{
    uint pad0;
    int isUsingCubemap;
    uint cubemapNumberOfMips;
    uint pad1;
    
    float4 ambientLightColorAndIntensity;
}

struct ModelVertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 bitangent : BITANGENT;
    float3 tangent : TANGENT;
};

struct ModelPixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLDPOSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 bitangent : BITANGENT;
    float3 tangent : TANGENT;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};