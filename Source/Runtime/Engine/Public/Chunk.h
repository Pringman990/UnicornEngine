#pragma once
#include "Voxel.h"
#include "SVO.h"
#include <vector>

//Dont forget to change in compute shader
#define CHUNK_SIZE 32

struct Chunk
{
	Voxel voxels[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

	Voxel& GetVoxel(uint32 x, uint32 y, uint32 z)
	{
		return voxels[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
	}

	Vector3 position;
	//SVO mSVO;

 //  static uint32_t PackColor(float r, float g, float b, float a) {
 //       // Ensure values are clamped between 0.0 and 1.0
 //       r = CLAMP(r, 0.0f, 1.0f);
 //       g = CLAMP(g, 0.0f, 1.0f);
 //       b = CLAMP(b, 0.0f, 1.0f);
 //       a = CLAMP(a, 0.0f, 1.0f);

 //       // Scale to 8-bit range [0, 255]
 //       uint32_t rInt = static_cast<uint32_t>(r * 255.0f);
 //       uint32_t gInt = static_cast<uint32_t>(g * 255.0f);
 //       uint32_t bInt = static_cast<uint32_t>(b * 255.0f);
 //       uint32_t aInt = static_cast<uint32_t>(a * 255.0f);

 //       // Pack into a single uint32 (RGBA layout)
 //       uint32_t packedColor = (aInt << 24) | (bInt << 16) | (gInt << 8) | rInt;

 //       return packedColor;
 //   }

	void RemoveOccludedVoxels()
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					Voxel& voxel = GetVoxel(x,y,z);
					if (!voxel.isSolid)
						continue;

					bool right = false;
					bool left = false;
					bool forward = false;
					bool back = false;
					bool top = false;
					bool bottom = false;

					if(x < CHUNK_SIZE-1)
						right = GetVoxel(x+1,y,z).isSolid;
					if (x > 0)
						left = GetVoxel(x-1,y,z).isSolid;
					
					if (z < CHUNK_SIZE - 1)
						forward = GetVoxel(x,y,z+1).isSolid;

					if (z > 0)
						back = GetVoxel(x,y,z-1).isSolid;

					if (y < CHUNK_SIZE - 1)
						top = GetVoxel(x,y+1,z).isSolid;

					if (y > 0)
						bottom = GetVoxel(x,y-1,z).isSolid;

					if (right && left && forward && back && top && bottom)
						voxel.isSolid = 0;
				}
			}
		}
	}
};