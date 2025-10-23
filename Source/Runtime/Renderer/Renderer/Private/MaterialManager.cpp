#include "MaterialManager.h"

#include "Renderer.h"
#include "ShaderManager.h"

MaterialManager::MaterialManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Init()
{
	{
		auto shaderHandle = mRenderer->GetShaderManager()->CreateShader("shaders://Mesh_VS.hlsl", "shaders://Single_Color_Mesh_FS.hlsl");
		AssetHandle<Material> materialHandle = CreateMaterial(shaderHandle);
		mEngineMaterials["single_color_opaque"] = materialHandle;
	}

	{
		auto shaderHandle = mRenderer->GetShaderManager()->CreateShader("shaders://Mesh_VS.hlsl", "shaders://Textured_Mesh_FS.hlsl");
		AssetHandle<Material> materialHandle = CreateMaterial(shaderHandle);
		mEngineMaterials["textured_mesh"] = materialHandle;
	}
}

AssetHandle<Material> MaterialManager::CreateMaterial(GPUResourceHandle<Shader> ShaderHandle)
{
	AssetHandle<Material> materialHandle = mMaterialPool.Allocate();

	Material* material = mMaterialPool.Get(materialHandle);
	material->shader = ShaderHandle;
	material->blend = MaterialBlendTypes::Opaque;

	return materialHandle;
}

AssetHandle<Material> MaterialManager::GetEngineMaterial(const String& Name)
{
	auto it = mEngineMaterials.find(Name);
	if (it != mEngineMaterials.end())
	{
		return it->second;
	}

	LOG_WARNING("No engine material with name: {}", Name);

	return AssetHandle<Material>::Invalid();
}
