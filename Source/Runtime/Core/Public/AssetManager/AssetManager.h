#pragma once
#include <EngineSubsystem.h>
#include "StandardTypes/StandardTypes.h"

#include "ResourceManagment/ResourceHandle.h"

#include "Internal/IAssetLoader.h"
#include "ResourceManagment/Internal/IResourcePool.h"
#include "ResourceManagment/ResourcePool.h"
#include "AssetLoader.h"

#include "FileSystem/FileSystem.h"
#include "Threading/ThreadPool.h"

#include "AssetManager/AssetBase.h"

class AssetManager : public EngineSubsystem<AssetManager>
{
	friend class EngineSubsystem<AssetManager>;
public:
	template<typename _Asset, typename _LoaderType>
	void RegisterLoader()
	{
		auto it = mLoaders.find(typeid(_Asset));
		if (it != mLoaders.end())
			_ASSERT_RENDERER(false, "Loader for asset type: {}, already exists!", typeid(_Asset).name());

		mLoaders[typeid(_Asset)] = OwnedPtr<IAssetLoader>(new _LoaderType());
	}

	template<typename _Asset>
	void RegisterPool()
	{
		auto it = mPools.find(typeid(_Asset));
		if (it != mPools.end())
			_ASSERT_RENDERER(false, "Pool for asset type: {}, already exists!", typeid(_Asset).name());

		mPools[typeid(_Asset)] = OwnedPtr<IResourcePool>(new ResourcePool<_Asset>());
	}

	template<typename T>
	void UnRegisterPool()
	{
		auto it = mPools.find(typeid(T));
		if (it == mPools.end())
			return;

		mPools.erase(it);
	}

	template<typename T>
	AssetHandle<T> LoadAsset(const String& VirtualPath, bool Threaded = true)
	{
		if (!FileSystem::Exists(VirtualPath))
		{
			_LOG_CORE_WARNING("No asset at path: {}", VirtualPath);
			return AssetHandle<T>::Invalid();
		}

		//TODO: if the asset already is loaded just give that back dont load again
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_THROW_CORE("No asset pool for type");
			return AssetHandle<T>::Invalid();
		}

		AssetHandle<T> handle = pool->Allocate();
		T* asset = AssetManager::Get()->GetAsset(handle);

		AssetLoader<T>* loader = GetLoader<T>();
		if (!loader)
		{
			_THROW_CORE("No asset loader for type");
			return AssetHandle<T>::Invalid();
		}

		if (Threaded)
		{
			ThreadPool::Get()->Enqueue([=]()
				{
					AssetLoadResult<T> result = loader->Load(VirtualPath, asset);

					asset->mMetadata = result.metadata;

					if constexpr (!std::is_same_v<decltype(result.resourceData), std::monostate>)
					{
						if constexpr (requires {asset->mResourceData; })
						{
							asset->mResourceData = result.resourceData;
						}
						else
						{
							static_assert(
								dependent_false_v<T>,
								"[Asset Error] Type T has ResourceData struct but is missing member mResourceData"
								);
						}
					}

					if constexpr (!std::is_same_v<decltype(result.stagingData), std::monostate>)
					{
						if constexpr (requires {asset->mStagingData; })
						{
							asset->mStagingData = result.stagingData;
						}
						else
						{
							static_assert(
								dependent_false_v<T>,
								"[Asset Error] Type T has StagingData struct but is missing member mStagingData"
								);
						}
					}

					asset->mState = AssetBase<T>::AssetState::Loaded;

				}, []() {});
		}
		else
		{
			AssetLoadResult<T> result = loader->Load(VirtualPath, asset);

			asset->mMetadata = result.metadata;

			if constexpr (!std::is_same_v<decltype(result.resourceData), std::monostate>)
			{
				if constexpr (requires {asset->mResourceData; })
				{
					asset->mResourceData = result.resourceData;
				}
				else
				{
					static_assert(
						dependent_false_v<T>,
						"[Asset Error] Type T has ResourceData struct but is missing member mResourceData"
						);
				}
			}

			if constexpr (!std::is_same_v<decltype(result.stagingData), std::monostate>)
			{
				if constexpr (requires {asset->mStagingData; })
				{
					asset->mStagingData = result.stagingData;
				}
				else
				{
					static_assert(
						dependent_false_v<T>,
						"[Asset Error] Type T has StagingData struct but is missing member mStagingData"
						);
				}
			}

			asset->mState = AssetBase<T>::AssetState::Loaded;
		}

		return handle;
	}

	template<typename T>
	T* GetAsset(const AssetHandle<T>& Handle)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No asset pool for type");
			return nullptr;
		}

		return pool->Get(Handle);
	}

private:
	AssetManager() {};
	~AssetManager() {};

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

