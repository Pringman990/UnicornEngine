#pragma once
#include <EngineSubsystem.h>
#include "StandardTypes/StandardTypes.h"

#include "AssetHandle.h"

#include "Internal/IAssetLoader.h"
#include "Internal/IAssetPool.h"
#include "AssetPool.h"
#include "AssetLoader.h"


class AssetManager : public EngineSubsystem<AssetManager>
{
	friend class EngineSubsystem<AssetManager>;
public:
	template<typename T>
	void RegisterLoader(IAssetLoader* Loader)
	{
		mLoaders[typeid(T)] = MakeOwned<T>(Loader);
	}

	template<typename T>
	void RegisterPool(IAssetPool* Pool)
	{
		mPools[typeid(T)] = MakeOwned<T>(Pool);
	}

	template<typename T>
	AssetHandle<T> LoadAsset(const String& Path)
	{
		AssetLoader<T>* loader = GetLoader<T>();
		if (!loader)
		{
			_ENSURE_CORE(loader, "No asset loader for type");
			return AssetHandle<T>::Invalid();
		}

		T asset = loader->Load(Path);

		AssetPool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No asset pool for type");
			return AssetHandle<T>::Invalid();
		}

		return pool->Add(asset);
	}

	template<typename T>
	T* GetAsset(const AssetHandle<T>& Handle)
	{
		AssetPool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No asset pool for type");
			return AssetHandle<T>::Invalid();
		}

		return pool->Get(Handle);
	}

private:
	AssetManager();
	~AssetManager();

	template<typename T>
	AssetPool<T>* GetPool()
	{
		auto it = mPools.find(typeid(T));
		if (it == mPools.end()) 
			return nullptr;

		return static_cast<AssetPool<T>*>(mPools[typeid(T)].get());
	}

	template<typename T>
	AssetLoader<T>* GetLoader()
	{
		auto it = mLoaders.find(typeid(T));
		if (it == mLoaders.end())
			return nullptr;

		return static_cast<AssetLoader<T>*>(mLoaders[typeid(T)].get());
	}
private:
	UnorderedMap<std::type_index, OwnedPtr<IAssetLoader>> mLoaders;
	UnorderedMap<std::type_index, OwnedPtr<IAssetPool>> mPools;
};

