#pragma once
#include <IAssetManager.h>
#include "Material.h"

class MaterialResourceManager : public IAssetManager<Material>
{
	friend class AssetRegistry;
	friend class Renderer;
public:

	virtual Material* LoadAsset() override;

	Material* CreateMaterial(const std::string& aName);
	Material* GetMaterial(const std::string& aName);

private:
	MaterialResourceManager();
	~MaterialResourceManager() override;

	void Init();
private:
	std::unordered_map<std::string, Material*> mMaterials;
	bool mHasInitilized;
};