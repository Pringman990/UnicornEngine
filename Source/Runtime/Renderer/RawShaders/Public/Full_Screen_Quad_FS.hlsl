Texture2D<float4> computeResult : register(t0);
SamplerState samp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION; // Required even if unused
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    return computeResult.Sample(samp, input.uv);
}