#include "Common.hlsli"

//BIND(0,0)
//cbuffer UniformBufferObject : register(b0)
//{
//    float4x4 model;
//    float4x4 worldToClip;
//};

//struct VSInput
//{
//    LOCATION(0)  float2 position : POSITION0;
//    LOCATION(1)  float4 color : COLOR0;
//};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    float2 positions[3] =
    {
        float2(0.0f, 0.5f), // top vertex
        float2(0.5f, -0.5f), // bottom right
        float2(-0.5f, -0.5f) // bottom left
    };

    // Corresponding vertex colors
    float4 colors[3] =
    {
        float4(1, 0, 0, 1), // red
        float4(0, 1, 0, 1), // green
        float4(0, 0, 1, 1) // blue
    };

    // Output position: xy from array, z=0, w=1
    output.Pos = float4(positions[vertexID], 0.0f, 1.0f);

    // Output vertex color
    output.Color = colors[vertexID];
    
    return output;
    
}