#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Material.h"

class Renderer;

class MaterialManager
{
public:
	MaterialManager(Renderer* InRenderer);
	~MaterialManager();

	void Init();

	AssetHandle<Material> CreateMaterial(GPUResourceHandle<Shader> ShaderHandle);

	AssetHandle<Material> GetEngineMaterial(const String& Name);

	inline Material* GetInternalMaterial(AssetHandle<Material> Handle)
	{
		return mMaterialPool.Get(Handle);
	}

private:
	Renderer* mRenderer;
	ResourcePool<Material> mMaterialPool;
	UnorderedMap<String, AssetHandle<Material>> mEngineMaterials;
};