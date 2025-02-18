#pragma once
#include "Voxel.h"
#include "Texture3D.h"
#include "Mesh.h"
#include "SVO.h"
#include "Octree.h"
#include <vector>
#include <Utility/Array3D.h>

//Dont forget to change in compute shader
#define CHUNK_SIZE_XZ 128
#define CHUNK_SIZE_Y 128

struct Chunk
{
	Texture3D* voxelsTexture = nullptr;
	std::shared_ptr<Mesh> cube;
	Octree* octree;
};

struct ChunkLoadData
{
	Array3D<uint8> voxelData;
	Vector3 position;
	Vector3 scale;
};