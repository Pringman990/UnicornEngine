#include "pch.h"
#include "ChunkGenerator.h"

#include <FastNoiseLite.h>

ChunkGenerator::ChunkGenerator()
{
}

ChunkGenerator::~ChunkGenerator()
{
}

std::vector<Chunk> ChunkGenerator::GenerateChunksFromPerlin(const uint32& aSize)
{
	FastNoiseLite fn;

	std::vector<Chunk> chunks(aSize * aSize);
	for (uint32 noiseY = 0; noiseY < aSize; noiseY++)
	{
		for (uint32 noiseX = 0; noiseX < aSize; noiseX++)
		{
			uint32 index = noiseY * aSize + noiseX;

			chunks[index].position = Vector3((float)(noiseX * CHUNK_SIZE), 0.0f, (float)(noiseY * CHUNK_SIZE)) * VOXEL_SIZE;

			for (int x = 0; x < CHUNK_SIZE; ++x)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
					float worldX = (float)(noiseX * CHUNK_SIZE + x) * VOXEL_SIZE;
					float worldZ = (float)(noiseY * CHUNK_SIZE + z) * VOXEL_SIZE;

					float noiseValue = fn.GetNoise(worldX, worldZ);
					int terrainHeight = static_cast<int32>((noiseValue + 1.0f) * 0.5f * CHUNK_SIZE);
					
					for (int y = 0; y < CHUNK_SIZE; ++y)
					{
						Voxel& voxel = chunks[index].GetVoxel(x, y, z);
						voxel.worldPosition = Vector3(worldX, (float)y * VOXEL_SIZE, worldZ);
						voxel.color = Color(1, 1, 1, 1.0f); //Default
						
						if (y <= terrainHeight)
						{
							voxel.isSolid = 1;
							if (y == terrainHeight)
							{
								voxel.color = Color(0.0f, 0.8f, 0.0f, 1.0f); // Green (RGBA)
							}
							else
							{
								voxel.color = Color(0.6f, 0.3f, 0.0f, 1.0f); // Brown (RGBA)
							}
						}
						else
						{
							voxel.isSolid = 0;
						}
					}
				}
			}
		}
	}


	return chunks;
}
