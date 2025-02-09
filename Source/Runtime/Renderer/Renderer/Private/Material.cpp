#include "pch.h"
#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

std::shared_ptr<Material> Material::CreateDefaultVoxel()
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    auto shader = Shader::CreateDefaultVoxel();
    material->SetShader(shader);
    return material;
}

std::shared_ptr<Material> Material::CreateDefaultPolygon()
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    auto shader = Shader::CreateDefaultPolygon();
    material->SetShader(shader);
    return material;
}

std::shared_ptr<Material> Material::Create(const std::string& aVSPath, const std::string& aPSPath)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    auto shader = Shader::Create(aVSPath, aPSPath);
    material->SetShader(shader);
    return material;
}

void Material::SetShader(std::shared_ptr<Shader>& aShader)
{
    mShader = aShader;
}

void Material::Bind()
{
    mShader->Bind();
    for (auto& [slot, texture] : mTextures)
    {
        texture->BindPS(slot);
    }
}

void Material::SetTexture(uint32 aSlot, Texture* aTexture)
{
	mTextures[aSlot] = aTexture;
}

Texture* Material::GetTexture(uint32 aSlot)
{
    return mTextures[aSlot];
}
