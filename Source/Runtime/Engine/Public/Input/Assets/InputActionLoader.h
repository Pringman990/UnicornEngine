#pragma once
#include "Assets/IAssetLoader.h"
#include "InputAction.h"

class InputActionLoader : public IAssetLoader
{
public:
	InputActionLoader();
	~InputActionLoader() override;

	virtual InputAction* CreateEmptyAsset(UniqueID128 UUID) override;
	virtual bool Load(AssetBase* Asset, const String& VirtualPath) override;
	virtual InputAction* ImportSource(const String& VirtualSourcePath) override;
	virtual InputAction* ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;

	virtual bool SupportsExtension(const String& Extension) override
	{
		return false;
	}

private:

};