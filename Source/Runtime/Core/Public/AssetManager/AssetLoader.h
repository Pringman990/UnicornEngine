#pragma once
#include "Internal/IAssetLoader.h"

template<typename T>
concept HasMetaData = requires { typename T::Metadata; };

template<HasMetaData T>
struct AssetLoadResult
{
	typename T::Metadata metadata;

	std::conditional_t <
		requires { typename T::ResourceData; },
	typename T::ResourceData,
		std::monostate > resourceData;
};

template<typename T>
class AssetLoader : public IAssetLoader
{
public:
	AssetLoader() = default;
	virtual ~AssetLoader() = default;

	virtual AssetLoadResult<T> Load(const String& VirtualPath, T* Asset) = 0;
	virtual bool Save(const String& VirtualPath, AssetHandle<T> Handle) = 0;

private:

};