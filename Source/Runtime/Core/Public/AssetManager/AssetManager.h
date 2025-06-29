#pragma once
#include <EngineSubsystem.h>
#include "StandardTypes/StandardTypes.h"

#include "ResourceManagment/ResourceHandle.h"

#include "Internal/IAssetLoader.h"
#include "ResourceManagment/Internal/IResourcePool.h"
#include "ResourceManagment/ResourcePool.h"
#include "AssetLoader.h"

#include "FileSystem/FileSystem.h"

class AssetManager : public EngineSubsystem<AssetManager>
{
	friend class EngineSubsystem<AssetManager>;
public:
	template<typename T>
	void RegisterLoader(IAssetLoader* Loader)
	{
		mLoaders[typeid(T)] = OwnedPtr<IAssetLoader>(Loader);
	}

	template<typename T>
	void RegisterPool(IResourcePool* Pool)
	{
		mPools[typeid(T)] = OwnedPtr<IResourcePool>(Pool);
	}

	template<typename T>
	ResourceHandle<T> LoadAsset(const String& VirtualPath)
	{
		if (!FileSystem::Exists(VirtualPath))
		{
			_LOG_CORE_ERROR("No asset at path: {}", VirtualPath);
			return ResourceHandle<T>::Invalid();
		}

		AssetLoader<T>* loader = GetLoader<T>();
		if (!loader)
		{
			_ENSURE_CORE(loader, "No asset loader for type");
			return ResourceHandle<T>::Invalid();
		}

		Optional<T> resource = loader->Load(VirtualPath);
		if (!resource)
		{
			_LOG_CORE_ERROR("Failed to create asset");
			return ResourceHandle<T>::Invalid();
		}

		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No asset pool for type");
			return ResourceHandle<T>::Invalid();
		}

		return pool->Add(resource);
	}

	template<typename T>
	T* GetAsset(const ResourceHandle<T>& Handle)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No asset pool for type");
			return ResourceHandle<T>::Invalid();
		}

		return pool->Get(Handle);
	}

private:
	AssetManager();
	~AssetManager();

	template<typename T>
	ResourcePool<T>* GetPool()
	{
		auto it = mPools.find(typeid(T));
		if (it == mPools.end()) 
			return nullptr;

		return static_cast<ResourcePool<T>*>(mPools[typeid(T)].get());
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
	UnorderedMap<std::type_index, OwnedPtr<IResourcePool>> mPools;
};

