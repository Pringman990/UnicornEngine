#include "pch.h"
#include "ChunkMesher.h"

#include <Mesh.h>
#include <Face.h>
#include <Vertex.h>

std::shared_ptr<Mesh> ChunkMesher::MeshifyChunk(Chunk& aChunk)
{
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_SIZE; ++y)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				auto& voxels = aChunk.GetVoxel(x,y,z);
				if (!voxels.isSolid)
					continue;
				
				AddFace(vertices, indices, (float)x, (float)y, (float)z, LEFT_FACE, voxels.color);
				AddFace(vertices, indices, (float)x, (float)y, (float)z, RIGHT_FACE, voxels.color);
				AddFace(vertices, indices, (float)x, (float)y, (float)z, BOTTOM_FACE, voxels.color);
				AddFace(vertices, indices, (float)x, (float)y, (float)z, TOP_FACE, voxels.color);
				AddFace(vertices, indices, (float)x, (float)y, (float)z, BACK_FACE, voxels.color);
				AddFace(vertices, indices, (float)x, (float)y, (float)z, FRONT_FACE, voxels.color);
			}
		}
	}

	std::shared_ptr<Mesh> mesh = Mesh::Create(vertices, indices);
	mesh->GetMatrix().Translation(aChunk.position);
	return mesh;
}

ChunkMesher::ChunkMesher()
{
}

ChunkMesher::~ChunkMesher()
{
}
