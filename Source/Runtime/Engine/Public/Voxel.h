#pragma once
#include <StandardTypes/StandardTypes.h>

enum class VoxelType : uint8
{
	eAir = 0,
	eDirt = 1,
};

//Dont forget to change in compute shader
#define VOXEL_SIZE 0.10f //10 cm

struct Voxel
{
	Vector4 color = {1,1,1,1};
	Vector3 worldPosition;
	uint32 isSolid = 0;
};