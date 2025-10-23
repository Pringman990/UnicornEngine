#include "BasicPrimitiveFactory.h"

#include "Renderer.h"
#include "MeshManager.h"
#include "MaterialManager.h"

#include "Vertex.h"

GPUResourceHandle<GPUMesh> BasicPrimitiveFactory::CreateCube()
{
	Vector<Vertex> vertices =
	{
		// Back face (Z = -0.5)
		{ {-0.5f, -0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {0,1} },  // 0
		{ {-0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {0,0} },  // 1
		{ { 0.5f,  0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {1,0} },  // 2
		{ { 0.5f, -0.5f, -0.5f, 1.0f}, {1, 0, 0, 1}, {1,1} },  // 3

		// Front face (Z = +0.5)
		{ {-0.5f, -0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {0,1} },  // 4
		{ {-0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {0,0} },  // 5
		{ { 0.5f,  0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {1,0} },  // 6
		{ { 0.5f, -0.5f,  0.5f, 1.0f}, {0, 1, 0, 1}, {1,1} },  // 7
	};

	// 12 triangles (two per face)
	Vector<uint32> indices =
	{
		// Back face
		0, 1, 2,   0, 2, 3,

		// Front face
		4, 6, 5,   4, 7, 6,

		// Left face
		4, 5, 1,   4, 1, 0,

		// Right face
		3, 2, 6,   3, 6, 7,

		// Top face
		1, 5, 6,   1, 6, 2,

		// Bottom face
		4, 0, 3,   4, 3, 7
	};

    Renderer* renderer = SubsystemManager::Get<Renderer>();
    return renderer->GetMeshManager()->CreateFromRaw(vertices, indices, renderer->GetMaterialManager()->GetEngineMaterial("textured_mesh"));
}
