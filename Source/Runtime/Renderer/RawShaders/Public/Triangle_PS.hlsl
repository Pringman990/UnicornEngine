
struct PSInput
{
    float4 Pos : SV_POSITION;
<<<<<<< feature/vulkan-vertexbuffer
    float4 Color : COLOR0;
=======
    float3 Color : COLOR0;
>>>>>>> main
};

float4 main(PSInput input) : SV_TARGET
{
<<<<<<< feature/vulkan-vertexbuffer
    return input.Color;
=======
    return float4(input.Color, 1.0f); // Output interpolated vertex color
>>>>>>> main
}