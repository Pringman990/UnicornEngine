
cbuffer MVPBuffer : register(b0)
{
    matrix MVP;
    matrix pad0;
    matrix pad1;
    matrix pad2;
};

struct VSInput
{
    float4 pos : POSITION;
    float3 col : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.pos = mul(MVP, input.pos);
    output.col = input.col;
    return output;
}