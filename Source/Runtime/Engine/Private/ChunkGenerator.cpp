#include "pch.h"
#include "ChunkGenerator.h"

#include <FastNoiseLite.h>
#include <Utility/Array3D.h>
#include <Utility/Utility.h>

ChunkGenerator::ChunkGenerator()
{
}

ChunkGenerator::~ChunkGenerator()
{
}

ChunkLoadData* ChunkGenerator::GenerateChunkFromPerlin(const int32& aWorldX, const int32& aWorldZ)
{
	FastNoiseLite fn;
	fn.SetFrequency(0.03f);

	ChunkLoadData* chunkData = new ChunkLoadData();
	chunkData->position = Vector3((float)aWorldX * VOXEL_SIZE, 0.0f, (float)aWorldZ * VOXEL_SIZE);
	chunkData->scale = Vector3(Vector3(CHUNK_SIZE_XZ * VOXEL_SIZE, CHUNK_SIZE_Y * VOXEL_SIZE, CHUNK_SIZE_XZ * VOXEL_SIZE));
	chunkData->voxelData.Resize(CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_SIZE_XZ);

	for (int x = 0; x < CHUNK_SIZE_XZ; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE_XZ; ++z)
		{
			float worldX = (aWorldX + x) * ((CHUNK_SIZE_XZ * VOXEL_SIZE) / CHUNK_SIZE_XZ);
			float worldZ = (aWorldZ  + z) * ((CHUNK_SIZE_XZ * VOXEL_SIZE) / CHUNK_SIZE_XZ);

			float noiseValue = fn.GetNoise(worldX, worldZ);
			int terrainHeight = static_cast<int>(((noiseValue + 1.0f) * 0.5f) * CHUNK_SIZE_Y) * 0.1f;

			for (int y = 0; y < CHUNK_SIZE_Y; ++y)
			{
				 if (y <= terrainHeight)
				{
				if (y < 100)
				{
					chunkData->voxelData.At(x, y, z) = RandomInt(1, 2); //water
				}
					if (y == terrainHeight && !(y < 110))
					{
						chunkData->voxelData.At(x, y, z) = RandomInt(6,8); //Green
					}
					else if (y < 110)
					{
						chunkData->voxelData.At(x, y, z) = RandomInt(4, 5); //Sand
					}
					else
					{
						chunkData->voxelData.At(x, y, z) = 9; //Brown
					}
				}
				else
				{
					chunkData->voxelData.At(x, y, z) = 0; // Air
				}
			}
		}
	}
	return chunkData;
}
