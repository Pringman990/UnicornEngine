#pragma once
#include "Vertex.h"

enum FaceType
{
	TOP_FACE,
	BOTTOM_FACE,
	RIGHT_FACE,
    FRONT_FACE,
	BACK_FACE,
	LEFT_FACE
};

void AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
    float x, float y, float z, FaceType faceType, Color /*color*/)
{
    Vector3 basePosition = Vector3(x, y, z) * VOXEL_SIZE; // Scale position
    Vector3 faceVertices[4];
    Vector3 normal;

    switch (faceType) {
    case LEFT_FACE:
        faceVertices[0] = basePosition + Vector3(0, 0, 0) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(0, 0, 1) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(0, 1, 1) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(0, 1, 0) * VOXEL_SIZE;
        normal = Vector3(-1, 0, 0);
        break;

    case RIGHT_FACE:
        faceVertices[0] = basePosition + Vector3(1, 0, 0) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(1, 1, 0) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(1, 1, 1) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(1, 0, 1) * VOXEL_SIZE;
        normal = Vector3(1, 0, 0);
        break;

    case TOP_FACE:
        faceVertices[0] = basePosition + Vector3(0, 1, 0) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(0, 1, 1) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(1, 1, 1) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(1, 1, 0) * VOXEL_SIZE;
        normal = Vector3(0, 1, 0);
        break;

    case BOTTOM_FACE:
        faceVertices[0] = basePosition + Vector3(0, 0, 0) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(1, 0, 0) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(1, 0, 1) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(0, 0, 1) * VOXEL_SIZE;
        normal = Vector3(0, -1, 0);
        break;

    case BACK_FACE:
        faceVertices[0] = basePosition + Vector3(0, 0, 0) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(0, 1, 0) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(1, 1, 0) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(1, 0, 0) * VOXEL_SIZE;
        normal = Vector3(0, 0, -1);
        break;

    case FRONT_FACE:
        faceVertices[0] = basePosition + Vector3(0, 0, 1) * VOXEL_SIZE;
        faceVertices[1] = basePosition + Vector3(1, 0, 1) * VOXEL_SIZE;
        faceVertices[2] = basePosition + Vector3(1, 1, 1) * VOXEL_SIZE;
        faceVertices[3] = basePosition + Vector3(0, 1, 1) * VOXEL_SIZE;
        normal = Vector3(0, 0, 1);
        break;
    }

    // Add vertices
    int indexStart = (int)vertices.size();
    for (int i = 0; i < 4; ++i) {
        Vector4 pos = Vector4(faceVertices[i], 1);
        vertices.push_back({ pos, Vector2()});
    }

    // Add indices (two triangles per face)
    indices.push_back(indexStart + 0);
    indices.push_back(indexStart + 1);
    indices.push_back(indexStart + 2);
    indices.push_back(indexStart + 2);
    indices.push_back(indexStart + 3);
    indices.push_back(indexStart + 0);
}