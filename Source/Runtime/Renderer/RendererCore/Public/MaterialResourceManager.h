#pragma once
#include <IResourceManager.h>
#include "Material.h"

class MaterialResourceManager : public IResourceManager
{
	friend class ResourceRegistry;
public:

	Material* CreateMaterial(const std::string& aName);
	Material* GetMaterial(const std::string& aName);

private:
	MaterialResourceManager();
	~MaterialResourceManager();
private:
	std::unordered_map<std::string, Material*> mMaterials;
};

//REGISTER_RESOURCEMANAGER(MaterialResourceManager)