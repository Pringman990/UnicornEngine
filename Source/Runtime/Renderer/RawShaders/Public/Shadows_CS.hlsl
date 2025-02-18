#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);

Texture2D GBufferAlbedoTexture : register(t0);
Texture2D GBufferNormalTexture : register(t1);
Texture2D GBufferWorldPositionTexture : register(t2);
Texture3D<uint> ChunksData : register(t3);

RWTexture2D<float4> OutputTexture : register(u0);

struct ChunkBounds
{
    float3 min;
    float3 max;
};

StructuredBuffer<ChunkBounds> ChunkBoundsBuffer : register(t4);

#define CHUNK_COUNT 10
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

float3 WorldPosToAtlasIndex(float3 worldPos)
{
    int chunkX = floor(worldPos.x / CHUNK_SIZE_XZ);
    int chunkZ = floor(worldPos.z / CHUNK_SIZE_XZ);

    // Ensure correct modulo behavior for negative coordinates
    int localX = ((int) worldPos.x % CHUNK_SIZE_XZ + CHUNK_SIZE_XZ) % CHUNK_SIZE_XZ;
    int localY = max(0, min((int) worldPos.y, CHUNK_SIZE_Y - 1));
    int localZ = ((int) worldPos.z % CHUNK_SIZE_XZ + CHUNK_SIZE_XZ) % CHUNK_SIZE_XZ;

    int atlasX = chunkX * CHUNK_SIZE_XZ + localX;
    int atlasY = localY;
    int atlasZ = chunkZ * CHUNK_SIZE_XZ + localZ;

    return int3(atlasX, atlasY, atlasZ);
}

bool RayMarchShadow(float3 worldPos, float3 sunDir)
{
   // int3 voxelCoords = WorldPosToAtlasIndex(worldPos);

    int chunkX = (int) floor(worldPos.x / CHUNK_SIZE_XZ);
    int chunkZ = (int) floor(worldPos.z / CHUNK_SIZE_XZ);
    int chunkIndex = chunkZ * CHUNK_COUNT + chunkX;
    float3 objectMinBounds = ChunkBoundsBuffer[chunkIndex].min;
    float3 objectMaxBounds = ChunkBoundsBuffer[chunkIndex].max;
    
    float3 rayOrigin = worldPos;
    float3 rayDir = normalize(sunDir);
    
    float tMin, tMax;
    if (!RayBoxIntersect(rayOrigin, rayDir, objectMinBounds, objectMaxBounds, tMin, tMax))
    {
        return false;
    }
    tMin = max(tMin, 0.0);
    
    float3 voxelSize = (objectMaxBounds - objectMinBounds) / float3(CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_SIZE_XZ);
    
    float3 startPos = rayOrigin + rayDir * (tMin + EPSILON);
    int3 voxelCoords = int3(floor((startPos - objectMinBounds) / voxelSize));
    voxelCoords = clamp(voxelCoords, int3(0, 0, 0), int3(CHUNK_SIZE_XZ - 1, CHUNK_SIZE_Y - 1, CHUNK_SIZE_XZ - 1));

    // **Voxel stepping**
    float3 deltaDist = abs(voxelSize / rayDir + EPSILON);
    int3 step = int3(sign(rayDir));
    float3 nextBoundary = (voxelCoords + (step > 0 ? 1 : 0)) * voxelSize + objectMinBounds;
    float3 tMaxDist = (nextBoundary - startPos) / rayDir;
    
    for (int i = 0; i < 512; i++) 
    {
        if (voxelCoords.x < 0 || voxelCoords.y < 0 || voxelCoords.z < 0 ||
            voxelCoords.x >= CHUNK_SIZE_XZ ||
            voxelCoords.y >= CHUNK_SIZE_Y ||
            voxelCoords.z >= CHUNK_SIZE_XZ)
        {
            return false; // Light reached, no shadow
        }
        
        uint voxelIndex = ChunksData.Load(int4(voxelCoords, 0));
        if (voxelIndex > 0)
        {
            return true; // Blocked
        }

        // Determine the next voxel to step into
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

    }

    return false; // No obstacles found
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 abledo = GBufferAlbedoTexture.Load(int3(dispatchThreadID.xy, 0));
    float4 normal = GBufferNormalTexture.Load(int3(dispatchThreadID.xy, 0));
    float4 worldPosition = GBufferWorldPositionTexture.Load(int3(dispatchThreadID.xy, 0));
  
    float3 rayOrigin = worldPosition.xyz;
    float3 rayDir = normalize(directionalLightDirection /** -1*/ );
  
    float shadowFactor = 1.0;
    if (RayMarchShadow(rayOrigin, rayDir))
    {
        shadowFactor = 0.3; // 30% of light still reaches surface (soft shadow)
    }

    //OutputTexture[dispatchThreadID.xy] = float4(shadowFactor, shadowFactor, shadowFactor, 1);
    OutputTexture[dispatchThreadID.xy] = abledo * directionalLightColorAndIntensity * directionalLightColorAndIntensity.a * shadowFactor;
}