#include "CommonAPI.hlsli"

cbuffer EngineFrameConstantBuffer : register(b0)
{
    float deltaTime;
    float3 _fpadding0;
}

cbuffer EngineCameraConstantBuffer : register(b1)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float3 cameraPosition;
    uint _cpad0;
}

cbuffer EngineObjectConstantBuffer : register(b2)
{
    float4x4 modelToWorld;
}