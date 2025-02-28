#pragma once
#include "Voxel.h"
#include "Texture3D.h"

#define BRICK_SIZE 16
#define OCTREE_SIZE 128

class StructuredBuffer;

class Octree
{
public:
	struct OctreeNode
	{
		std::unique_ptr<OctreeNode> children[8];
		Texture3D* texture = nullptr;
		bool isLeaf = false;
	};
public:
	Octree();
	~Octree();

	void Build(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aChunkPosition);
	void DrawDebug(const Vector3& aRootPosition);

	void BindFlattenedOctreeToPS(const uint8& aSlot);
private:
	std::unique_ptr<OctreeNode> BuildInternal(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aPosition, const Vector3& aChunkPosition);
	bool HasSolidVoxel(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aPosition, const Vector3& aChunkPosition);

	int32 FlattenOctree(Octree::OctreeNode* aNode);

	void DrawDebugRecursive(const std::unique_ptr<OctreeNode>& aNode, int aNodeSize, const Vector3& aPosition);

private:
	std::unique_ptr<OctreeNode> mRootNode;

	std::vector<int32> mFlattenedOctree;
	std::unordered_map<Octree::OctreeNode*, int32> mNodeToFlatIndex;

	StructuredBuffer* mChunkOctree;
};