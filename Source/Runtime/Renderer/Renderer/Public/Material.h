#pragma once
#include "Texture.h"
#include "Shader.h"

class Material
{
public:
	Material();
	~Material();

	static std::shared_ptr<Material> CreateDefaultVoxel();
	static std::shared_ptr<Material> CreateDefaultPolygon();
	static std::shared_ptr<Material> Create(const std::string& aVSPath, const std::string& aPSPath);

	void SetShader(std::shared_ptr<Shader>& aShader);
	void Bind();
	void SetTexture(uint32 aSlot, Texture* aTexture);
	Texture* GetTexture(uint32 aSlot);

private:
	std::shared_ptr<Shader> mShader;
	std::map<uint32, Texture*> mTextures;
};