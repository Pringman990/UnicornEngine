#pragma once
#include <Core.h>
#include "AssetBase.h"

/**
* Interface for asset loader which the AssetRegistry will own.
*/
class IAssetLoader
{
public:
	ENGINE_API virtual ~IAssetLoader() = default;
	ENGINE_API virtual AssetBase* CreateEmptyAsset(UniqueID128 UUID) = 0;
	ENGINE_API virtual AssetBase* ImportSource(const Path& VirtualSourcePath) = 0;
	ENGINE_API virtual AssetBase* ReimportSource(const Path& VirtualSourcePath, UniqueID128 UUID) = 0;
	ENGINE_API virtual bool Load(AssetBase* Asset, const Path& VirtualPath) = 0;
	ENGINE_API virtual bool UnLoad(const Path& VirtualPath) = 0;
	ENGINE_API virtual bool SupportsExtension(const String& Extension) = 0;

private:

};