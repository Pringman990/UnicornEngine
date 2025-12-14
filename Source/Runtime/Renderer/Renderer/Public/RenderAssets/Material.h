#pragma once
#include <EngineMinimal.h>
#include <Assets/AssetBase.h>
#include <Assets/AssetRef.h>
#include "RendererMinimal.h"

#include "Shader.h"
#include "GPUResources/GPUMaterial.h"

class Texture2D;

using MaterialParameterVariant = Variant<
	//AssetRef<Texture2D>
	UniqueID128
>;

enum class MaterialParameterType
{
	Undefined,
	Texture2D
};

struct MaterialParameter
{
	MaterialParameterType type;
	MaterialParameterVariant value;
};

class Material : public AssetBase
{
public:
	Material();
	Material(UniqueID128 UUID);
	~Material() override;

	void MarkClean() { mIsDirty = false; }
	void MarkDirty() { mIsDirty = true; }
	bool IsDirty() const { return mIsDirty; }

	void SetTexture(const String& Param, Texture2D* Texture);

	void SetGPUMaterialHandle(GPUResourceHandle<GPUMaterial> Handle) { mGPUMaterial = Handle; };
	GPUResourceHandle<GPUMaterial> GetGPUMaterialHandle() const { return mGPUMaterial; };

	const UnorderedMap<String, MaterialParameter>& GetShaderParams() const { return mShaderParameters; };

	void AddMaterialParameter(const String& Name, MaterialParameterType Type, const MaterialParameterVariant& Data) { mShaderParameters.insert({ Name, {Type, Data} }); };

private:
	bool DoParamExist(const String& Name);
private:
	GPUResourceHandle<GPUMaterial> mGPUMaterial;
	UnorderedMap<String, MaterialParameter> mShaderParameters;
	bool mIsDirty;
};