#pragma once
#include "Shader.h"

class MaterialShader : public Shader
{
public:
	MaterialShader();
	~MaterialShader() override;

	bool Bind() override;

	void SetVertexShader(Shader* aVertexShader);
	void SetPixelShader(Shader* aPixelShader);

	Shader* GetVertexShader() const;
	Shader* GetPixelShader() const;

private:
	Shader* mVertexShader;
	Shader* mPixelShader;
};