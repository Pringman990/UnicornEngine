#include "pch.h"
#include "Octree.h"

#include "Chunk.h"
#include <Utility/Array3D.h>
#include <StructuredBuffer.h>

Octree::Octree()
{
}

Octree::~Octree()
{
}

void Octree::Build(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aChunkPosition)
{
	mRootNode = BuildInternal(someVoxels, aNodeSize, aChunkPosition, aChunkPosition);
	if (mRootNode)
	{
		FlattenOctree(mRootNode.get());
		mChunkOctree = StructuredBuffer::Create(sizeof(int32), (uint32)mFlattenedOctree.size(), mFlattenedOctree.data());
	}
}

void Octree::DrawDebug(const Vector3& aRootPosition)
{
	if (mRootNode)
		DrawDebugRecursive(mRootNode, CHUNK_SIZE_XZ, aRootPosition);
}

void Octree::BindFlattenedOctreeToPS(const uint8& aSlot)
{
	mChunkOctree->BindToPS(aSlot);
}

std::unique_ptr<Octree::OctreeNode> Octree::BuildInternal(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aPosition, const Vector3& aChunkPosition)
{
	std::unique_ptr<Octree::OctreeNode> node = std::make_unique<OctreeNode>();

	if (aNodeSize == BRICK_SIZE)
	{
		Array3D<uint8> textureData(BRICK_SIZE, BRICK_SIZE, BRICK_SIZE);

		Vector3 nodeMinCorner = aPosition - Vector3(BRICK_SIZE * 0.5f * VOXEL_SIZE);
		Vector3 localMinCorner = (nodeMinCorner - aChunkPosition) / VOXEL_SIZE;
		for (int32 x = 0; x < BRICK_SIZE; x++)
		{
			for (int32 y = 0; y < BRICK_SIZE; y++)
			{
				for (int32 z = 0; z < BRICK_SIZE; z++)
				{
					int32 wx = static_cast<int32>(localMinCorner.x) + x;
					int32 wy = static_cast<int32>(localMinCorner.y) + y;
					int32 wz = static_cast<int32>(localMinCorner.z) + z;

					// Ensure bounds are correct
					if (wx < 0 || wx >= CHUNK_SIZE_XZ ||
						wy < 0 || wy >= CHUNK_SIZE_Y ||
						wz < 0 || wz >= CHUNK_SIZE_XZ
						)
						continue;

					// Convert 3D index to 1D index
					int32 indexChunk = wx + wy * CHUNK_SIZE_XZ + wz * CHUNK_SIZE_XZ * CHUNK_SIZE_Y;
					int32 indexTexture = x + y * BRICK_SIZE + z * BRICK_SIZE * BRICK_SIZE;

					textureData[indexTexture] = someVoxels[indexChunk];
				}
			}
		}
		node->texture = Texture3D::Create(textureData);
		node->isLeaf = true;
		return node;
	}

	if (!HasSolidVoxel(someVoxels, aNodeSize, aPosition + (Vector3((aNodeSize / 2) * VOXEL_SIZE)), aChunkPosition))
	{
		return node;
	}

	//Split node in 8 children
	int32 childSize = aNodeSize / 2;
	for (int32 i = 0; i < 8; i++)
	{
		//check if child has any solid voxles in it 
		//if so split it and run again.
		Vector3 childOffset(
			((i & 1) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE,
			((i & 2) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE,
			((i & 4) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE
		);
		Vector3 childPosition = (aPosition + childOffset);

		node->children[i] = BuildInternal(someVoxels, childSize, childPosition, aChunkPosition);
	}

	return node;
}

bool Octree::HasSolidVoxel(const Array3D<uint8>& someVoxels, int aNodeSize, const Vector3& aPosition, const Vector3& aChunkPosition)
{
	// Convert aPosition (center of node) to bottom-left corner of node
	Vector3 nodeMinCorner = aPosition - Vector3(aNodeSize * 0.5f * VOXEL_SIZE);

	// Convert to local voxel space
	Vector3 localMinCorner = (nodeMinCorner - aChunkPosition) / VOXEL_SIZE;

	for (int32 x = 0; x < aNodeSize; x++)
	{
		for (int32 y = 0; y < aNodeSize; y++)
		{
			for (int32 z = 0; z < aNodeSize; z++)
			{
				int32 wx = static_cast<int32>(localMinCorner.x) + x;
				int32 wy = static_cast<int32>(localMinCorner.y) + y;
				int32 wz = static_cast<int32>(localMinCorner.z) + z;

				// Ensure bounds are correct
				if (wx < 0 || wx >= CHUNK_SIZE_XZ || wy < 0 || wy >= CHUNK_SIZE_Y || wz < 0 || wz >= CHUNK_SIZE_XZ)
					continue;

				// Convert 3D index to 1D index
				int32 index = wx + wy * CHUNK_SIZE_XZ + wz * CHUNK_SIZE_XZ * CHUNK_SIZE_Y;

				// If there's a solid voxel, return true
				if (someVoxels[index] != 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

int32 Octree::FlattenOctree(Octree::OctreeNode* aNode)
{
	if (!aNode)
		return -1;

	int32 nodeIndex = (int32)mFlattenedOctree.size();
	mFlattenedOctree.resize(nodeIndex + 8, -1);

	mNodeToFlatIndex[aNode] = nodeIndex;

	if (aNode->isLeaf)
	{
		for (int32 i = 0; i < 8; i++)
		{
			mFlattenedOctree[nodeIndex + i] = -10;
		}
		return nodeIndex;
	}

	for (int32 i = 0; i < 8; i++)
	{
		if (aNode->children[i])
		{
			mFlattenedOctree[nodeIndex + i] = FlattenOctree(aNode->children[i].get());
		}
	}

	return nodeIndex;
}

void Octree::DrawDebugRecursive(const std::unique_ptr<OctreeNode>& aNode, int aNodeSize, const Vector3& aPosition)
{
	if (!aNode) return;

	float ratio = (static_cast<float>(aNodeSize) / CHUNK_SIZE_XZ); // 1.0 at root, 0.0 at smallest
	float red = (1.0f - ratio);   // More red for smaller nodes
	float green = ratio;

	Renderer::GetInstance()->DrawDebugCube(aPosition + Vector3((aNodeSize / 2) * VOXEL_SIZE), Vector3(aNodeSize * VOXEL_SIZE), Color(red, green, 0));

	int32 childSize = aNodeSize / 2;
	for (int32 i = 0; i < 8; i++)
	{
		Vector3 childOffset(
			((i & 1) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE,
			((i & 2) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE,
			((i & 4) ? 1.0f : 0.0f) * childSize * VOXEL_SIZE
		);
		Vector3 childPosition = (aPosition + childOffset);
		DrawDebugRecursive(aNode->children[i], childSize, childPosition);
	}
}