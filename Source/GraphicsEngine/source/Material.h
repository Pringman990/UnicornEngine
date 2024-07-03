#pragma once
#include "source/Texture.h"

enum class eMaterialTexture
{
	eColor,
	eNormal,
	eMaterial
};

class Material
{
public:
	Material();
	~Material();

	bool CreateMaterial(const std::string& aColorPath, const std::string& aMaterialPath, const std::string& aNormalPath);

	void Bind();

	const Texture* GetTexture(eMaterialTexture aTexture) const;

private:
	Texture mColorTexture;		//_c
	Texture mNormalTexture;		//_n
	Texture mMaterialTexture;	//_m
};

