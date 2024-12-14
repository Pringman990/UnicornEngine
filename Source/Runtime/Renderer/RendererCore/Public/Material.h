#pragma once
#include "Shader/MaterialShader.h"
#include "Shader/InputLayout.h"

class Material
{
	friend class MaterialResourceManager;
public:

	bool Bind();

	const eInputLayoutStandardTypes GetInputLayoutType() const;
	InputLayout* GetInputLayout();

	void SetShader(MaterialShader* aShader) { mMaterialShader = aShader; };
	MaterialShader* GetShader() { return mMaterialShader; };
private:
	Material();
	~Material();
protected:
	InputLayout* mInputLayout;
	MaterialShader* mMaterialShader;
};