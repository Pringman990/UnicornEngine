#include "Common.hlsli"

SamplerState DefaultSampler : register(s0);
Texture3D<uint1> VoxelData : register(t0);
Texture3D<uint1> ChunkTextures : register(t1);

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

uint CheckVoxelTextureForHit(float3 rayOrigin, float3 rayDirection, float tMin, float3 startPosition)
{
    float3 voxelSize = float3(objectMaxBounds - objectMinBounds) / float3(CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_SIZE_XZ);
    
    //world-space->boundary->voxel-space
    int3 voxelTexCoord = int3(floor((startPosition - objectMinBounds) / voxelSize));
    voxelTexCoord = clamp(voxelTexCoord, int3(0, 0, 0), int3(CHUNK_SIZE_XZ - 1, CHUNK_SIZE_Y - 1, CHUNK_SIZE_XZ - 1));

    //How much time passes before crossing voxel boundary
    float3 stepSize = abs(voxelSize / rayDirection + EPSILON);
    int3 stepDirection = int3(sign(rayDirection));
    
    //Find the first boundary in each axis
    float3 initialBoundary = (voxelTexCoord + (stepDirection > 0 ? 1 : 0)) * voxelSize + objectMinBounds;
    
    //Time when we hit the next boundary along all axis
    float3 tMaxDistance = (initialBoundary - startPosition) / rayDirection;
    
    for (int i = 0; i < 512; i++)
    {
        uint voxelTexColorIndex = VoxelData.Load(int4(voxelTexCoord, 0));

        //Check if voxel is solid (0 = air)
        if (voxelTexColorIndex > 0)
        {
            return voxelTexColorIndex;
        }
        
        if (tMaxDistance.x < tMaxDistance.y && tMaxDistance.x < tMaxDistance.z)
        {
            voxelTexCoord.x += stepDirection.x;
            tMaxDistance.x += stepSize.x;
        }
        else if (tMaxDistance.y < tMaxDistance.z)
        {
            voxelTexCoord.y += stepDirection.y;
            tMaxDistance.y += stepSize.y;
        }
        else
        {
            voxelTexCoord.z += stepDirection.z;
            tMaxDistance.z += stepSize.z;
        }

        if (voxelTexCoord.x < 0 ||
            voxelTexCoord.y < 0 ||
            voxelTexCoord.z < 0 ||
            voxelTexCoord.x >= CHUNK_SIZE_XZ ||
            voxelTexCoord.y >= CHUNK_SIZE_Y ||
            voxelTexCoord.z >= CHUNK_SIZE_XZ
            )
        {
            return 0;
        }
    }
    return 0;
}

GBufferOutput main(MeshPixelInput input)
{
    GBufferOutput result;
    result.albedo = float4(0,0,0,0);
    result.normal = float4(0,0,0,0);
    result.worldPosition = float4(0,0,0,0);
    result.depth = float(0);

    float3 rayOrigin = cameraPosition;
    float3 rayDirection = input.worldPosition.xyz - cameraPosition;
    
    float tMin = 0; //Enter time
    float tMax = 0; //Exit time
    
    //Check if we hit the chunk
    if (!RayBoxIntersect(rayOrigin, rayDirection, objectMinBounds, objectMaxBounds, tMin, tMax))
        discard;
    
     //Find world start position of ray in chunk
    float3 startPosition = rayOrigin + rayDirection * (tMin + EPSILON);
    
    uint hitVoxelTexIndex = CheckVoxelTextureForHit(rayOrigin, rayDirection, tMin, startPosition);
    if (hitVoxelTexIndex != 0)
    {
        result.albedo = colors[hitVoxelTexIndex];
            
        if (tMin < EPSILON)
            tMin = 0.0f;
            
        float4 clipPosition = mul(worldToClipMatrix, float4(startPosition, 1.0f));
        if (clipPosition.w < EPSILON)
            clipPosition.w = EPSILON;
            
        result.depth = clipPosition.z / clipPosition.w;
        if (tMin == 0.0f)
            result.depth = saturate(result.depth - 0.0001f);
            
        result.worldPosition = float4(startPosition, 1.0f);
        return result;
    }
    
      
    
    discard;
    return result;
}