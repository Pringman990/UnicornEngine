#pragma once
#include <Core.h>
#include "ResourceManagment/ResourceHandle.h"

/**
* Interface for asset managers which the AssetRegistry will own.
*/
class IAssetManager
{
public:
	virtual ~IAssetManager() = default;
	virtual GenericHandle ImportSource(const String& VirtualSourcePath) = 0;
	virtual GenericHandle ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) = 0;
	virtual GenericHandle Load(const String& VirtualPath) = 0;
	virtual bool UnLoad(const String& VirtualPath) = 0;
	virtual UniqueID128 FindAssetBySource(const String& VirtualSourcePath) = 0;
	virtual bool Exists(const String& VirtualPath) const = 0;

private:

};