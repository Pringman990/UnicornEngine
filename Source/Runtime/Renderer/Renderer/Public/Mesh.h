#pragma once
#include "Vertex.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
class Shader;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Draw();

	static std::shared_ptr<Mesh> Create(std::vector<Vertex>& someVertices, std::vector<uint32>& someIndices);
	static std::shared_ptr<Mesh> Create(
		std::vector<Vertex>& someVertices,
		std::vector<uint32>& someIndices, 
		const std::string& aVSPath, 
		const std::string& aPSPath
	);

	Matrix& GetMatrix();

private:
	uint32 mIndexCount;
	ComPtr<ID3D11Buffer> mVertexBuffer;
	ComPtr<ID3D11Buffer> mIndexBuffer;

	std::shared_ptr<Shader> mShader;

	Matrix mTransform;
};