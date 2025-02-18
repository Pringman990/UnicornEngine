#include "Common.hlsli"

LinePixelInput main(LineVertexInput input)
{
    LinePixelInput output;
    float4 vertexClipPos = mul(worldToClipMatrix, float4(input.position.xyz , 1));
    output.position = vertexClipPos;
    output.color = input.color;
    return output;
}