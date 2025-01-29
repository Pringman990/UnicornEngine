cbuffer CameraConstantBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    uint pad;
}

RWTexture2D<float4> resultTexture : register(u0);

struct Voxel
{
    float4 color;
    float3 worldPosition;
    uint isSolid;
};
StructuredBuffer<Voxel> voxels : register(t0);

#define CHUNK_SIZE 32
#define VOXEL_SIZE 0.10f
#define FLT_MAX 3.40282347e+38f

bool RayIntersectsAABB(float3 rayOrigin, float3 rayDirection, float3 boxMin, float3 boxMax, out float tMin, out float tMax)
{
    tMin = 0.0f;
    tMax = FLT_MAX;

    // Iterate over each axis (X, Y, Z)
    for (int i = 0; i < 3; i++)
    {
        if (abs(rayDirection[i]) < 1e-8f)
        {
            // Ray is parallel to the slab; check if the origin is outside
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                return false;
        }
        else
        {
            // Compute intersection distances for near and far planes
            float t1 = (boxMin[i] - rayOrigin[i]) / rayDirection[i];
            float t2 = (boxMax[i] - rayOrigin[i]) / rayDirection[i];

            // Ensure t1 is the smaller and t2 is the larger value
            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            // Update tMin and tMax
            tMin = max(tMin, t1);
            tMax = min(tMax, t2);

            // If tMin exceeds tMax, there is no intersection
            if (tMin > tMax)
                return false;
        }
    }

    return true;
}

// Unproject the NDC screen space to world space
float3 Unproject(float2 screenPos, float nearPlane, float farPlane)
{
   // Convert from NDC to clip space (because NDC is [-1,1] and clip space is [x, y, z, w])
    //We flip Y because screen space i top left (1,1) but ndc goes between [-1,1] with top being +1 and bottom -1
    float4 clipSpacePos = float4(screenPos.x, screenPos.y * -1, 1.0f, 1.0f);
    float4 worldPos = mul(worldToClipMatrix, clipSpacePos);
    
    return worldPos.xyz / worldPos.w;
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Get the screen position in NDC (normalized device coordinates)
    float2 screenPos = float2(dispatchThreadID.x, dispatchThreadID.y) / float2(1200, 720);
    screenPos = screenPos * 2.0f - 1.0f;

    float3 rayDirection = Unproject(screenPos, 0.001f, 1000.0f);
    rayDirection = normalize(rayDirection - cameraPosition);

    float3 rayOrigin = cameraPosition;
    float closestT = FLT_MAX;
    float4 voxelColor = float4(0, 0, 0, 0);
    
    for (uint i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE); i++)
    {
        Voxel v = voxels[i];
      
        if (!v.isSolid)
            continue;
      
        float3 boxMin = voxels[i].worldPosition - 0.5f * VOXEL_SIZE;
        float3 boxMax = voxels[i].worldPosition + 0.5f * VOXEL_SIZE;
    
        float tMin, tMax;
        if (RayIntersectsAABB(rayOrigin, rayDirection, boxMin, boxMax, tMin, tMax))
        {
            if (tMin < closestT)
            {
                closestT = tMin;
                voxelColor = voxels[i].color;
            }
        }
    }
    
    resultTexture[dispatchThreadID.xy] = voxelColor;
}