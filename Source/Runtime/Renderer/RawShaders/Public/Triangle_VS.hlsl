
struct VSOutput
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR0;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;

    // Procedural triangle positions
    float2 positions[3] =
    {
        float2(0.0f, -0.5f),
        float2(0.5f, 0.5f),
        float2(-0.5f, 0.5f)
    };

    // Vertex colors for each corner
    float3 colors[3] =
    {
        float3(1.0f, 0.0f, 0.0f), // Red
        float3(0.0f, 1.0f, 0.0f), // Green
        float3(0.0f, 0.0f, 1.0f) // Blue
    };

    output.Pos = float4(positions[vertexID], 0.0f, 1.0f);
    output.Color = colors[vertexID];
    return output;
}