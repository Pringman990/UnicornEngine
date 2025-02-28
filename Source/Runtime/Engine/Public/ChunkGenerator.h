#pragma once
#include "Chunk.h"

class ChunkGenerator
{
public:
	static ChunkLoadData* GenerateChunkFromPerlin(const int32& aWorldX, const int32& aWorldZ);
	static void GenerateChunksThreaded(const uint32& aChunkCount, std::function<void(std::vector<Chunk>)> aCallback);
private:
	ChunkGenerator();
	~ChunkGenerator();
private:
};