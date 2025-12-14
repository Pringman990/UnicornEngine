#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Assets/IAssetLoader.h"
#include "Material.h"

class MaterialManager : public IAssetLoader
{
public:
	MaterialManager();
	~MaterialManager() override;

	virtual Material* Load(const String& VirtualPath) override;
	virtual Material* ImportSource(const String& VirtualSourcePath) override;
	virtual Material* ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;

	virtual bool SupportsExtension(const String& Extension) override
	{
		return false;
	}

private:
};