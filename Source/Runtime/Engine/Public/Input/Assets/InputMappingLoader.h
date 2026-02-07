#pragma once
#include "Assets/IAssetLoader.h"
#include "InputMapping.h"

class InputMappingLoader : public IAssetLoader
{
public:
	InputMappingLoader();
	~InputMappingLoader() override;

	virtual InputMapping* CreateEmptyAsset(UniqueID128 UUID) override;
	virtual bool Load(AssetBase* Asset, const String& VirtualPath) override;
	virtual InputMapping* ImportSource(const String& VirtualSourcePath) override;
	virtual InputMapping* ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;

	virtual bool SupportsExtension(const String& Extension) override
	{
		return false;
	}

private:

};