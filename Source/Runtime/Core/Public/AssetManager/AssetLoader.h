#pragma once
#include "Internal/IAssetLoader.h"

template<typename T>
class AssetLoader : public IAssetLoader
{
public:
	AssetLoader() = default;
	virtual ~AssetLoader() = default;

	virtual Optional<T> Load(const String& VirtualPath) = 0;
	virtual bool Save(const String& VirtualPath, SharedPtr<T> Data) = 0;

private:

};