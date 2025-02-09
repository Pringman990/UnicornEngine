#pragma once
#include "Chunk.h"

class ChunkGenerator
{
public:
	static ChunkLoadData* GenerateChunkFromPerlin(const int32& aWorldX, const int32& aWorldZ);

private:
	ChunkGenerator();
	~ChunkGenerator();
};