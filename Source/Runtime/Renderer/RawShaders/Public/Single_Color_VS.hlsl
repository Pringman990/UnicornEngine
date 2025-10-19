
struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput input, uint vertexID : SV_VertexID)
{
    VSOutput output;
    //output.position = input.position;
    
    //float3 colors[3] =
    //{
    //    float3(1, 0, 0), // Red
    //    float3(0, 1, 0), // Green
    //    float3(0, 0, 1) // Blue
    //};

    //float speed = 0.5; // rotations per second
    //float phase = time * speed * 3.0; // 3 vertices per cycle

    //int current = int(floor(phase)) % 3;
    //int next = (current + 1) % 3;
    //float fracPart = frac(phase);

    //// rotate colors along the triangle
    //int index = vertexID % 3;
    //int colorIndex0 = (index + current) % 3;
    //int colorIndex1 = (index + next) % 3;

    //output.color.rgb = lerp(colors[colorIndex0], colors[colorIndex1], fracPart);
    //output.color.a = 1.0;
    output.color = float4(1,0,0,1);
    output.position = float4(0,0,0,1);
    return output;
}