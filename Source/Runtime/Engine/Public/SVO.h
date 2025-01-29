#pragma once

struct SVONode
{
    Vector3 position;
    uint32_t childIndex;      // Index of the first child in the node array
    uint32_t voxelData;       // Voxel material data (e.g., color/material ID)
    uint8_t childMask;        // Indicates which children exist (8 bits)
    uint8_t visibilityMask;   // Visibility mask for voxel faces (+X, -X, +Y, -Y, +Z, -Z)
    bool isLeaf;              // Indicates if the node is a leaf

};

class SVO
{
public:
	SVO();
	~SVO();

    void AddNode(const Vector3& aPosition, const uint32& aVoxelData);

    std::vector<SVONode>& GetNodes();

private:
    SVONode CreateEmptyNode();
    void SubdivideNode(const uint32& aNodeIndex);
    uint8 ComputeVisibilityMask(const Vector3& aPosition);
private:
    std::vector<SVONode> mNodes;

};