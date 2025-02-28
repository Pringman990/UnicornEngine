#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
Texture3D<uint1> VoxelData : register(t0);

struct GBufferOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 worldPosition : SV_Target2;
    float depth : SV_Depth;
};

#define CHUNK_SIZE_XZ 128
#define CHUNK_SIZE_Y 128
#define EPSILON 1e-4

bool RayBoxIntersect(float3 rayOrigin, float3 rayDir, float3 boxMin, float3 boxMax, out float tMin, out float tMax)
{
    float3 t0 = (boxMin - rayOrigin) / (rayDir + EPSILON);
    float3 t1 = (boxMax - rayOrigin) / (rayDir + EPSILON);

    float3 tMinVec = min(t0, t1);
    float3 tMaxVec = max(t0, t1);

    tMin = max(max(tMinVec.x, tMinVec.y), tMinVec.z);
    tMax = min(min(tMaxVec.x, tMaxVec.y), tMaxVec.z);
    
    if (tMin < 0.0)
        tMin = 0.0;

    return (tMax > tMin) && (tMax > 0);

}

GBufferOutput main(MeshPixelInput input)
{
    GBufferOutput result;
    result.albedo = float4(0, 0, 0, 1);
    
    float3 rayOrigin = cameraPosition;
    float3 rayDir = normalize(input.worldPosition.xyz - cameraPosition);
    
    // Ray-box intersection
    float tMin = 0;
    float tMax = 0;
    
    if (!RayBoxIntersect(rayOrigin, rayDir, objectMinBounds, objectMaxBounds, tMin, tMax))
        discard;
    
    tMin = max(tMin, 0.0);
    
    // Convert to voxel space
    float3 voxelSize = (objectMaxBounds - objectMinBounds) / float3(CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_SIZE_XZ);
    
    float3 startPos = rayOrigin + rayDir * (tMin + EPSILON);
    int3 voxelCoords = int3(floor((startPos - objectMinBounds) / voxelSize));
    voxelCoords = clamp(voxelCoords, int3(0, 0, 0), int3(CHUNK_SIZE_XZ - 1, CHUNK_SIZE_Y - 1, CHUNK_SIZE_XZ - 1));

    // Compute voxel stepping
    float3 deltaDist = abs(voxelSize / rayDir + EPSILON);
    int3 step = int3(sign(rayDir));

    float3 nextBoundary = (voxelCoords + (step > 0 ? 1 : 0)) * voxelSize + objectMinBounds;
    float3 tMaxDist = (nextBoundary - startPos) / rayDir;

    // Ray marching loop
    for (int i = 0; i < 512; i++)
    {
        uint voxelIndex = VoxelData.Load(int4(voxelCoords, 0));
    
        if (voxelIndex > 0)
        {
            result.albedo = colors[voxelIndex];
            
            if (tMin < EPSILON)
                tMin = 0.0;
            
            float3 hitPosition = rayOrigin + rayDir * tMin;
            float4 clipPos = mul(worldToClipMatrix, float4(hitPosition, 1.0));
            
            if (clipPos.w < EPSILON)
                clipPos.w = EPSILON;
            
            result.depth = clipPos.z / clipPos.w;
            if (tMin == 0.0)
                result.depth = saturate(result.depth - 0.0001);
            
            result.worldPosition = float4(hitPosition, 1.0);
            return result;
        }
        
        if (tMaxDist.x < tMaxDist.y && tMaxDist.x < tMaxDist.z)
        {
            voxelCoords.x += step.x;
            tMaxDist.x += deltaDist.x;
        }
        else if (tMaxDist.y < tMaxDist.z)
        {
            voxelCoords.y += step.y;
            tMaxDist.y += deltaDist.y;
        }
        else
        {
            voxelCoords.z += step.z;
            tMaxDist.z += deltaDist.z;
        }
        
        if (voxelCoords.x < 0 || voxelCoords.y < 0 || voxelCoords.z < 0 ||
        voxelCoords.x >= CHUNK_SIZE_XZ || voxelCoords.y >= CHUNK_SIZE_Y || voxelCoords.z >= CHUNK_SIZE_XZ)
        {
            discard;
        }
    }

    discard;
    return result;
}