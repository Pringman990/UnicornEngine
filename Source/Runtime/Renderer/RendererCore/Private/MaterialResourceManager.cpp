#include "pch.h"
#include "MaterialResourceManager.h"

#include "Shader/ShaderResourceManager.h"
#include "Shader/InputLayoutManager.h"
#include "Renderer.h"

#include "Texture/TextureResourceManager.h"

MaterialResourceManager::MaterialResourceManager()
{
	CreateMaterial("__UNCE_Defualt_Material");
}

MaterialResourceManager::~MaterialResourceManager()
{
	for (auto [name, material] : mMaterials)
	{
		delete material;
		material = nullptr;
	}
	mMaterials.clear();
}

Material* MaterialResourceManager::CreateMaterial(const std::string& aName)
{
	auto it = mMaterials.find(aName);
	if (it != mMaterials.end())
	{
		_LOG_RENDERER_ERROR("Material with name: {}, already exist");
		return nullptr;
	}

	ShaderResourceManager* shaderManager = GET_RESOURCE_MANAGER(ShaderResourceManager);
	Shader* shader = shaderManager->TryGetShader(eShaderTypes::eMatrial, "__UNCE_Defualt_Material");
	MaterialShader* materialShader = static_cast<MaterialShader*>(shader);

	Material* material = new Material();
	material->SetShader(materialShader);

	InputLayoutManager* layoutManager = Renderer::GetInstance()->GetInputLayoutManager();
	material->mInputLayout = layoutManager->TryGetLayout(eInputLayoutStandardTypes::eModel, materialShader->GetVertexShader());
	
	TextureResourceManager* textureManager = GET_RESOURCE_MANAGER(TextureResourceManager);
	Texture* texture0 = textureManager->LoadTextureFromFile("../../Assets/Textures/defaultMaterial_c.dds");
	material->AddTexture(0, texture0);

	mMaterials.insert({aName, material});

	return material;
}

Material* MaterialResourceManager::GetMaterial(const std::string& aName)
{
	auto it = mMaterials.find(aName);
	if (it == mMaterials.end())
	{
		_LOG_RENDERER_ERROR("Material with name: {}, does not exist");
		return nullptr;
	}

	return it->second;
}
