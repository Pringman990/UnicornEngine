#pragma once
#include "Shader.h"

struct MeshData;

class ModelShader : public Shader
{
public:
	ModelShader();
	~ModelShader();

	void Render(const MeshData& someMeshData, const Transform& aTransform);

private:
	bool Init() override;
	bool CreateInputLayout(const std::string& aVertexShader) override;
private:
	std::unique_ptr<ConstantBuffer> mObjectBuffer;
};