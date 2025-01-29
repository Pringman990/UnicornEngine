#include "pch.h"
#include "SVO.h"

SVO::SVO()
{
   // mNodes.reserve(1024);
    mNodes.push_back(CreateEmptyNode());
}

SVO::~SVO()
{
}

void SVO::AddNode(const Vector3& aPosition, const uint32& aVoxelData)
{
    uint32_t currentNodeIndex = 0;
    SVONode* currentNode = &mNodes[currentNodeIndex];

    // Descend into the tree to find the appropriate leaf node
    for (int32 depth = 0; depth < 5; ++depth) { // Limit depth for 32x32x32 chunks
       
        int32 childIdx = 0;
        
        if ((int32)aPosition.x & (1 << (4 - depth)))
            childIdx |= 1;
        
        if ((int32)aPosition.y & (1 << (4 - depth)))
            childIdx |= 2;
        
        if ((int32)aPosition.z & (1 << (4 - depth)))
            childIdx |= 4;

        if (!(currentNode->childMask & (1 << childIdx))) {
            // If the child doesn't exist, subdivide the node
            SubdivideNode(currentNodeIndex);
        }

        // Move to the child node
        currentNodeIndex = currentNode->childIndex + childIdx;
        currentNode = &mNodes[currentNodeIndex];
    }

    // Set the voxel data for the final leaf node
    currentNode->isLeaf = true;
    currentNode->voxelData = aVoxelData;
    currentNode->visibilityMask = ComputeVisibilityMask(aPosition);
    currentNode->position = aPosition;
}

std::vector<SVONode>& SVO::GetNodes()
{
    return mNodes;
}

SVONode SVO::CreateEmptyNode()
{
	return {Vector3(),0,0,0,0,true};
}

void SVO::SubdivideNode(const uint32& aNodeIndex)
{
	SVONode& node = mNodes[aNodeIndex];
	if (!node.isLeaf)
	{
		_LOG_ENGINE_WARNING("Cant subdivide non leaf node, Index: {}", aNodeIndex);
		return;
	}

	node.isLeaf = false;
	node.childMask = 0xFF;
	//node.childIndex = (uint32)mNodes.size();

	for (int32 i = 0; i < 8; i++)
	{
		mNodes.push_back(CreateEmptyNode());
	}
    node.childIndex = (uint32)(mNodes.size() - 8);
}

uint8 SVO::ComputeVisibilityMask(const Vector3& /*aPosition*/)
{
	return 0b111111;
}