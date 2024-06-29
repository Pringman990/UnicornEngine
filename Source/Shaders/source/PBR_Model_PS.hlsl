#include "common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(ModelPixelInputType input)
{
    PixelOutput output;

    float2 scaledUV = input.uv;
    
    float3 toEye = normalize(cameraPosition.xyz - input.worldPosition.xyz);
    float4 albedo = colorTexture.Sample(defaultSampler, scaledUV).rgba;
    
    float3 normal = normalTexture.Sample(defaultSampler, scaledUV).wyz;
    float ambientOcclusion = normal.b;
    
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);
    
    float3x3 TBN = float3x3(normalize(input.tangent.xyz), normalize(-input.bitangent.xyz), normalize(input.normal.xyz));
    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));
    
    float3 material = materialTexture.Sample(defaultSampler, scaledUV).rgb;
    float metalness = material.r;
    float roughness = material.g;
    float emissive = material.b;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 ambience = EvaluateAmbiance(
    cubeMap,
    pixelNormal,
    input.normal.xyz,
    toEye,
    roughness,
    ambientOcclusion,
    diffuseColor,
    specularColor
    );
    
    float3 directionalLight = EvaluateDirectionalLight(
    diffuseColor,
    specularColor,
    pixelNormal,
    roughness,
    directionalLightColorAndIntensity.rgb,
    directionalLightDirection.xyz,
    toEye.xyz
    );
    
    float3 pointLightsSum = 0; // <- The sum of all point lights.
    for (unsigned int p = 0; p < mumPointLights; p++)
    {
        pointLightsSum += EvaluatePointLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			pointLights[p].color.rgb, pointLights[p].color.w, pointLights[p].radius, pointLights[p].position.xyz,
			toEye.xyz, input.worldPosition.xyz);
    }
    
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambience + directionalLight + pointLightsSum + emissiveAlbedo;
    
    output.color.rgb = radiance;
    output.color.a = albedo.a;
    return output;
}