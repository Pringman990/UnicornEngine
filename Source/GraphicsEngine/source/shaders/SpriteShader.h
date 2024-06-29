#pragma once
#include "Shader.h"

class SpriteShader : public Shader
{
public:
	SpriteShader();
	~SpriteShader();

	void Render(const Transform& aTransform);

private:
	bool Init() override;
	bool CreateInputLayout(ID3DBlob* aVertexBlob) override;
private:
	uint32_t mVertexCount;
	uint32_t mIndexCount;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	std::unique_ptr<ConstantBuffer> mObjectBuffer;
};