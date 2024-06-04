#define MAX_NUMBER_OF_LIGHTS 8

SamplerState defaultSampler : register(s0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D materialTexture : register(t3);
TextureCube cubeMap : register(t0);

cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    int garbage0;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelToWorld;
}

cbuffer LightConstantBuffer : register(b2)
{
    struct PointLight
    {
        float3 position;
        float radius;
        float4 color; // a = intensity
    } pointLights[MAX_NUMBER_OF_LIGHTS];
    unsigned int mumPointLights;
    unsigned int pad3;
    unsigned int pad4;
    
    int numberOfMips;
    float4 ambientLightColorAndIntensity;
    float4 directionalLightDirection;
    float4 directionalLightColorAndIntensity;
    
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 bitangent : BITANGENT;
    float3 tangent : TANGENT;
};

struct PixelInputType
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