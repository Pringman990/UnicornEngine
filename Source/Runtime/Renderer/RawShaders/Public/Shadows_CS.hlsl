#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);

Texture2D GBufferAlbedoTexture : register(t0);
Texture2D GBufferNormalTexture : register(t1);
Texture2D GBufferWorldPositionTexture : register(t2);
Texture3D<uint> ChunksData : register(t3);

RWTexture2D<float4> OutputTexture : register(u0);

#define CHUNK_COUNT 10
#define CHUNK_SIZE_XZ 128
#define CHUNK_SIZE_Y 256

float3 WorldPosToAtlasIndex(float3 worldPos)
{
    // Compute chunk indices
    int chunkX = (int) floor(worldPos.x / CHUNK_SIZE_XZ);
    int chunkZ = (int) floor(worldPos.z / CHUNK_SIZE_XZ);

    // Convert world position to local voxel coordinates
    int localX = ((uint) worldPos.x % CHUNK_SIZE_XZ + CHUNK_SIZE_XZ) % CHUNK_SIZE_XZ;
    int localY = (uint) worldPos.y;
    int localZ = ((uint) worldPos.z % CHUNK_SIZE_XZ + CHUNK_SIZE_XZ) % CHUNK_SIZE_XZ;

    // Compute the global atlas position
    int atlasX = chunkX * CHUNK_SIZE_XZ + localX;
    int atlasY = localY;
    int atlasZ = chunkZ * CHUNK_SIZE_XZ + localZ;

    return int3(atlasX, atlasY, atlasZ);
}

bool RayMarchShadow(float3 worldPos, float3 sunDir)
{
    // Convert world position to voxel index
    int3 voxelIndex = WorldPosToAtlasIndex(worldPos);

    // Define voxel stepping parameters
    float3 deltaDist = abs(1.0 / (sunDir + 1e-5)); // Prevent div by zero
    int3 step = int3(sign(sunDir));

    // Compute voxel boundary intersections
    float3 nextBoundary = (voxelIndex + (step > 0 ? 1 : 0));
    float3 tMax = (nextBoundary - worldPos) / sunDir;
    
    // March through voxels
    for (int i = 0; i < 128; i++)  // Reduce iteration count
    {
        uint voxel = ChunksData.Load(int4(voxelIndex, 0));

        if (voxel != 0)
        {
            return true; // Blocked
        }

        // Determine the next voxel to step into
        if (tMax.x < tMax.y && tMax.x < tMax.z)
        {
            voxelIndex.x += step.x;
            tMax.x += deltaDist.x;
        }
        else if (tMax.y < tMax.z)
        {
            voxelIndex.y += step.y;
            tMax.y += deltaDist.y;
        }
        else
        {
            voxelIndex.z += step.z;
            tMax.z += deltaDist.z;
        }

        // Early exit if out of bounds
        if (voxelIndex.x < 0 || voxelIndex.y < 0 || voxelIndex.z < 0 ||
            voxelIndex.x >= CHUNK_COUNT * CHUNK_SIZE_XZ ||
            voxelIndex.y >= CHUNK_SIZE_Y ||
            voxelIndex.z >= CHUNK_COUNT * CHUNK_SIZE_XZ)
        {
            return false; // Light reached, no shadow
        }
    }

    return false; // No obstacles found
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    //float4 abledo = GBufferAlbedoTexture.Load(int3(dispatchThreadID.xy, 0));
    //float4 normal = GBufferNormalTexture.Load(int3(dispatchThreadID.xy, 0));
    //float4 worldPosition = GBufferWorldPositionTexture.Load(int3(dispatchThreadID.xy, 0));
  
    //float3 rayOrigin = worldPosition.xyz;
    //float3 rayDir = normalize(-directionalLightDirection);
  
    //float shadowFactor = 1.0;
    //if (RayMarchShadow(rayOrigin, rayDir))
    //{
    //    shadowFactor = 0.0; // 30% of light still reaches surface (soft shadow)
    //}

    //OutputTexture[dispatchThreadID.xy] = abledo * directionalLightColorAndIntensity * directionalLightColorAndIntensity.a * shadowFactor;

    // Sample G-buffer textures
    float4 albedo = GBufferAlbedoTexture.Load(int3(dispatchThreadID.xy, 0));
    float4 worldPosition = GBufferWorldPositionTexture.Load(int3(dispatchThreadID.xy, 0));

    // Reconstruct depth from world position
    float depth = worldPosition.z; // Assuming Z is depth
    depth = saturate(depth / 100.0f); // Normalize depth (adjust divisor for scene scale)

    // Blend color with depth to prevent incorrect transparency
    float depthFactor = 1.0 - depth; // Invert depth so closer pixels are stronger
    float3 finalColor = albedo.rgb * depthFactor;

    // Write final color (no need for separate depth texture)
    OutputTexture[dispatchThreadID.xy] = float4(finalColor, 1.0);
}