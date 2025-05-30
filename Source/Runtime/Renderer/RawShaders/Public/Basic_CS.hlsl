RWTexture2D<float4> outputTex : register(u0, space0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    outputTex[DTid.xy] = float4(1, 0, 0, 1); // Fill red for now
}