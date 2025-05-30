struct PSInput
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

float4 main(PSInput input) : SV_Target
{
    return float4(input.col, 1.0);
}