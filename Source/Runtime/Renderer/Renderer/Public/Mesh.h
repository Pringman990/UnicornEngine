#pragma once
#include "Vertex.h"
#include "Material.h"
#include <Math/Transform.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
class Shader;

struct ObjectBounds
{
	Vector3 min;
	Vector3 max;
};

struct SubMesh
{
	std::shared_ptr<Material> material = nullptr;
	uint32 startIndex = 0;
	uint32 indexCount = 0;
};

class Mesh
{
	friend class MeshLoader;
public:
	Mesh();
	~Mesh();

	void Draw();
	void SetMaterial(uint32 aSubMeshIndex, std::shared_ptr<Material>& aMaterial);
	std::shared_ptr<Material> GetMaterial(uint32 aSubMeshIndex);
	Transform& GetTransform();

	static std::shared_ptr<Mesh> CreateCube();
	static std::shared_ptr<Mesh> CreateSphereInvertedNormals();
	static std::shared_ptr<Mesh> Create(std::vector<Vertex>& someVertices, std::vector<uint32>& someIndices);


private:
	std::vector<SubMesh> mSubMeshes;
	ComPtr<ID3D11Buffer> mVertexBuffer;
	ComPtr<ID3D11Buffer> mIndexBuffer;

	Transform mTransform;
	ObjectBounds mBounds;

	/**
	* We use vertex count for displaying in editor.
	* Not used for rendering as we use DrawIndexed and not Draw.
	*/
	uint32 mVertexCount;
};