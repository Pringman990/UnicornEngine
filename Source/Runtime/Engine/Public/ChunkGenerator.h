#pragma once
#include "Chunk.h"

class ChunkGenerator
{
public:
	static std::vector<Chunk> GenerateChunksFromPerlin(const uint32& aSize);

private:
	ChunkGenerator();
	~ChunkGenerator();
};