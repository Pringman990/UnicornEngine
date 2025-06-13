#pragma once
#include "Internal/IAssetLoader.h"

template<typename T>
class AssetLoader final : public IAssetLoader
{
public:
	AssetLoader() = default;
	virtual ~AssetLoader() = default;

	virtual T Load(const String& Path) = 0;
	virtual bool Save(const String& Path, SharedPtr<T> Data) = 0;

private:

};