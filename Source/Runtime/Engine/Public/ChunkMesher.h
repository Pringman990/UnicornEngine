#pragma once
#include "Chunk.h"

class Mesh;

class ChunkMesher
{
public:

	static std::shared_ptr<Mesh> MeshifyChunk(Chunk& aChunk);

private:
	ChunkMesher();
	~ChunkMesher();
};