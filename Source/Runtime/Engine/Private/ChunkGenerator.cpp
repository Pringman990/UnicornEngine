#include "pch.h"
#include "ChunkGenerator.h"

#include <FastNoiseLite.h>
#include <Utility/Array3D.h>
#include <Utility/Utility.h>
#include <Threading/ThreadPool.h>

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
	chunkData->scale = Vector3(CHUNK_SIZE_XZ * VOXEL_SIZE, CHUNK_SIZE_Y * VOXEL_SIZE, CHUNK_SIZE_XZ * VOXEL_SIZE);
	chunkData->voxelData.Resize(CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_SIZE_XZ);

	for (int x = 0; x < CHUNK_SIZE_XZ; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE_XZ; ++z)
		{
			float worldX = (aWorldX + x) * ((CHUNK_SIZE_XZ * VOXEL_SIZE) / CHUNK_SIZE_XZ);
			float worldZ = (aWorldZ + z) * ((CHUNK_SIZE_XZ * VOXEL_SIZE) / CHUNK_SIZE_XZ);

			float noiseValue = fn.GetNoise(worldX, worldZ);
			int terrainHeight = static_cast<int>(((noiseValue + 1.0f) * 0.5f) * CHUNK_SIZE_Y) * VOXEL_SIZE;

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
						chunkData->voxelData.At(x, y, z) = RandomInt(6, 8); //Green
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

void ChunkGenerator::GenerateChunksThreaded(const uint32& aChunkCount, std::function<void(std::vector<Chunk>)> aCallback)
{
	ThreadPool::GetInstance()->Enqueue(
		[=]()
		{
			std::vector<ChunkLoadData*> data(aChunkCount * aChunkCount);
			for (int32 x = 0; x < aChunkCount; x++)
			{
				for (int32 z = 0; z < aChunkCount; z++)
				{
					data[x * aChunkCount + z] = ChunkGenerator::GenerateChunkFromPerlin(x * CHUNK_SIZE_XZ, z * CHUNK_SIZE_XZ);
				}
			}
			return data;
		},
		[callback = std::move(aCallback)](std::vector<ChunkLoadData*> ChunkData)
		{
			std::vector<Chunk> chunks;
			for (int32 i = 0; i < ChunkData.size(); i++)
			{
				Chunk chunk;

				chunk.cube = Mesh::CreateCube();
				std::shared_ptr<Shader> chunkShader = Shader::CreateDefaultChunk();
				chunk.cube->GetMaterial(0)->SetShader(chunkShader);

				chunk.cube->GetTransform().SetPosition(ChunkData[i]->position);
				chunk.cube->GetTransform().SetScale(ChunkData[i]->scale);

				Texture3D* texture = Texture3D::Create(ChunkData[i]->voxelData);
				chunk.cube->GetMaterial(0)->SetTexture(0, texture);
				chunk.voxelsTexture = texture;

				chunk.octree = new Octree();
				chunk.octree->Build(ChunkData[i]->voxelData, 128, ChunkData[i]->position);

				delete ChunkData[i];
				chunks.push_back(chunk);
			}

			if (callback)
				callback(chunks);
		}
	);
}
