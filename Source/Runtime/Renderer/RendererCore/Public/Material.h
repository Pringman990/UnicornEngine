#pragma once
#include "Shader/MaterialShader.h"
#include "Shader/InputLayout.h"

#include <StandardTypes.h>
#include <unordered_map>

class Texture;

class Material
{
	friend class MaterialResourceManager;
public:

	void BindTextures();

	const eInputLayoutStandardTypes GetInputLayoutType() const;
	InputLayout* GetInputLayout();

	void SetShader(MaterialShader* aShader) { mMaterialShader = aShader; };
	MaterialShader* GetShader() { return mMaterialShader; };

	void AddTexture(uint32 aSlot, Texture* aTexture);
private:
	Material();
	~Material();
protected:
	InputLayout* mInputLayout;
	MaterialShader* mMaterialShader;

	std::unordered_map<uint32, Texture*> mTextures;
};