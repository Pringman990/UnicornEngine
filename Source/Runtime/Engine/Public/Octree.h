#pragma once
#include "Voxel.h"

class Octree
{
public:
	struct OctreeNode
	{
		OctreeNode* children[8];
		bool isLeaf;
	};
public:
	Octree();
	~Octree();

private:
	OctreeNode mRootNode;
};