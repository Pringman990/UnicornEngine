#pragma once

class Transform;
class Material;

struct ID3D11Buffer;

struct MeshData
{
	uint32_t vertexCount;
	uint32_t indexCount;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	std::shared_ptr<Material> material;
};

class Model
{
public:

public:
	Model();
	~Model();

	void Init(std::vector<MeshData>& someMeshData);

	void BindMaterials();

	void SetMeshData(int aMeshIndex, MeshData someData);

	bool SetMaterial(
		int aMeshIndex,
		const std::string& aColorePath,
		const std::string& aNormalPath,
		const std::string& aMaterialPath
	);

	Transform& GetTransform() { return mTransform; };
	const std::vector<MeshData>& GetMeshData() { return mMeshData; };

private:
	std::vector<MeshData> mMeshData;
	Transform mTransform;
};