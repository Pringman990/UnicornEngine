#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUMaterial.h"

class Renderer;
class Material;

class GPUMaterialManager
{
public:
	GPUMaterialManager(Renderer* InRenderer);
	~GPUMaterialManager();

	void Init();

	GPUResourceHandle<GPUMaterial> CreateMaterialFromProgram(GPUResourceHandle<ShaderProgram> ShaderHandle);

	GPUResourceHandle<GPUMaterial> GetEngineMaterial(const String& Name);

	inline GPUMaterial* GetInternalMaterial(GPUResourceHandle<GPUMaterial> Handle)
	{
		return mMaterialPool.Get(Handle);
	}

	void UpdateFromAsset(GPUResourceHandle<GPUMaterial> GPU, Material* CPU);

	const GPUResourceHandle<GPUMaterial>& GetInvalid() const { return mInvalid; }

private:
	Renderer* mRenderer;
	ResourcePool<GPUMaterial> mMaterialPool;
	UnorderedMap<String, GPUResourceHandle<GPUMaterial>> mEngineMaterials;

	GPUResourceHandle<GPUMaterial> mInvalid;
};