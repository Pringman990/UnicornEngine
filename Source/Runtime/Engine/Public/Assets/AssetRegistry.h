#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>

#include "IAssetLoader.h"
#include "ResourceManagment/AssetResourcePool.h"

#include "FileSystem/FileSystem.h"

#include <YAML/YamlHelper.h>

enum class AssetLoadResult
{
	Failed,
	AssetLoaded,
	SourceImported,
	SourceReimported
};

template<typename T>
struct AssetLoadData
{
	T* asset;
	AssetLoadResult result = AssetLoadResult::Failed;
};

struct AssetFileReadData
{
	String Type;
	String Name;
	UniqueID128 UUID;
	OwnedPtr<Archive> CustomData = nullptr;
	String SourcePath;
};

/*
* Engine subsystem
*/
class AssetRegistry
{
	friend struct subsystem::SubsystemDescriptor;
public:

	ENGINE_API static AssetRegistry* Instance();

	/**
	* Used to pre register all uuids when starting the engine.
	*/
	ENGINE_API void FindAndRegisterAllAssets();

	template<typename T>
	void RegisterLoader(const Vector<std::type_index>& AssetTypes, const Vector<String>& Extensions)
	{
		std::type_index id = typeid(T);

		T* loader = GetLoader<T>();
		if (loader)
		{
			LOG_WARNING("Asset Manager is already registered, type id name: {}", id.name());
			return;
		}

		mLoaders[id] = MakeOwned<T>();

//		for (auto& extension : Extensions)
//		{
//#ifdef _DEBUG
//			auto it = mExtensionToLoader.find(extension);
//			if (it != mExtensionToLoader.end())
//			{
//				THROW("Extension already has registered manager: {}", extension);
//			}
//#endif // _DEBUG
//
//			mExtensionToLoader[extension] = loader;
//		}

		for (auto& type : AssetTypes)
		{
#ifdef _DEBUG
			auto it = mTypeToLoader.find(type);
			if (it != mTypeToLoader.end())
			{
				THROW("Asset type already has registered manager: {}", type.name());
			}
#endif // _DEBUG

			mTypeToLoader[type] = mLoaders[id].get();
			mTypeStringToLoader[ExtractTypeInfoNameWithoutSpecifier(type.name())] = mLoaders[id].get();
		}
	}

	template<typename T>
	AssetRef<T> Load(const Path& VirtualPath)
	{
		IAssetLoader* loader = GetLoaderFromAssetType<T>();
		if (!loader)
		{
			LOG_WARNING("Trying to get asset without a loader, path: {}", VirtualPath);
			return AssetRef<T>::MakeInvalid();
		}

		String extension = ExtractExtension(VirtualPath);
		if (extension == "asset")
		{
			//If the extension was an asset then we want to check if it's already loaded and if it is just return that version.
			auto it = mPathToUUID.find(VirtualPath);
			if (it != mPathToUUID.end())
			{
				return AssetRef<T>(mUUIDToHandle[it->second]);
			}

			//If the asset was not loaded yet then allocate and load it.
			AssetBase* asset = loader->Load(VirtualPath);
			if (!asset)
			{
				LOG_WARNING("Asset failed to load, path: {}", VirtualPath);
				return AssetRef<T>::MakeInvalid();
			}

			AssetRefHandle handle = mAssets.Allocate(asset);
			mPathToUUID[VirtualPath] = asset->GetUUID();
			mUUIDToHandle[asset->GetUUID()] = handle;

			return AssetRef<T>(handle);
		}

		return AssetRef<T>::MakeInvalid();
	}

	template<typename T>
	AssetRef<T> Load(const UniqueID128& UUID)
	{
		auto it = mUUIDToHandle.find(UUID);
		if (it != mUUIDToHandle.end())
		{
			return it->second;
		}
		return AssetRef<T>::MakeInvalid();
	}

	template<typename T>
	T* GetAsset(const AssetRef<T>& Handle)
	{
		return mAssets.template Get<T>(Handle);
	}

	template<typename T>
	AssetRef<T> GetAssetFromUUID(const UniqueID128& UUID)
	{
		auto it = mUUIDToHandle.find(UUID);
		if (it != mUUIDToHandle.end())
		{
			return it->second;
		}
		return AssetRef<T>::MakeInvalid();
	}

	template<typename T>
	Vector<T*> GetAssetsOfType(const String& Type)
	{
		Vector<T*> result;

		const auto& entries = mAssets.GetEntries();
		for (uint32 i = 0; i < entries.size(); i++)
		{
			if (entries[i].asset && entries[i].asset->GetType() == Type)
				result.push_back(static_cast<T*>(entries[i].asset.get()));
		}

		return result;
	}

	/**
	* Creates a YAML asset file at VirtualPath location.
	* SourcePath can be used if the asset has external data like mesh asset files has fbx files as source.
	*/
	ENGINE_API static void CreateAssetFile(
		const String& VirtualPath,
		const String& Type,
		const String& Name,
		const UniqueID128& UUID,
		const YAML::Node CustomData = {},
		const String& SourcePath = ""
	);

	ENGINE_API static AssetFileReadData ReadAssetFile(const String& VirtualPath);

	template<typename... Ext>
	static Vector<String> MakeExtensions(Ext&&... ext)
	{
		Vector<String> extensions;

		if constexpr (sizeof...(ext) > 0)
		{
			extensions.reserve(sizeof...(ext));
			(extensions.emplace_back(std::forward<Ext>(ext)), ...);
		}

		return extensions;
	}

	template <typename... Ts>
	static Vector<std::type_index> MakeTypeIndexVector()
	{
		return { std::type_index(typeid(Ts))... };
	}

private:
	AssetRegistry();
	~AssetRegistry();

	void Load(const Path& VirtualPath, const String& Type)
	{
		IAssetLoader* loader = GetLoaderFromAssetTypeString(Type);
		if (!loader)
		{
			LOG_WARNING("Trying to get asset without a loader, path: {}", VirtualPath);
			return;
		}

		String extension = ExtractExtension(VirtualPath);
		if (extension == "asset")
		{
			//If the extension was an asset then we want to check if it's already loaded and if it is just return that version.
			auto it = mPathToUUID.find(VirtualPath);
			if (it != mPathToUUID.end())
			{
				return;
			}

			//If the asset was not loaded yet then allocate and load it.
			AssetBase* asset = loader->Load(VirtualPath);
			if (!asset)
			{
				LOG_WARNING("Asset failed to load, path: {}", VirtualPath);
				return;
			}

			AssetRefHandle handle = mAssets.Allocate(asset);
			mPathToUUID[VirtualPath] = asset->GetUUID();
			mUUIDToHandle[asset->GetUUID()] = handle;

			return;
		}
	}

	template<typename T>
	T* GetLoader()
	{
		std::type_index id = typeid(T);
		auto it = mLoaders.find(id);
		if (it == mLoaders.end())
		{
			return nullptr;
		}

		return static_cast<T*>(it->second.get());
	}

	template<typename T>
	IAssetLoader* GetLoaderFromAssetType()
	{
		std::type_index id = typeid(T);
		auto it = mTypeToLoader.find(id);
		if (it == mTypeToLoader.end())
		{
			LOG_WARNING("Asset loader was not found in registry for asset type: {}", id.name());
			return nullptr;
		}

		return it->second;
	}

	IAssetLoader* GetLoaderFromAssetTypeString(const String& Type)
	{
		auto it = mTypeStringToLoader.find(Type);
		if (it == mTypeStringToLoader.end())
		{
			LOG_WARNING("Asset loader was not found in registry for asset type: {}", Type);
			return nullptr;
		}

		return it->second;
	}

private:
	static AssetRegistry* sInstance;

	UnorderedMap<std::type_index, OwnedPtr<IAssetLoader>> mLoaders;
	UnorderedMap<std::type_index, IAssetLoader*> mTypeToLoader;
	UnorderedMap<String, IAssetLoader*> mTypeStringToLoader;
	
	UnorderedMap<UniqueID128, AssetRefHandle> mUUIDToHandle;
	UnorderedMap<Path, UniqueID128> mPathToUUID;
	AssetResourcePool<AssetBase> mAssets;
};

#define UNWRAP(...) __VA_ARGS__

/**
* The args passed will be the registered extensions for the manager.
*
* @ingroup
* AssetRegistry.
*/
#define REGISTER_ASSET_LOADER(CLASS, ASSETS, SUPPORTED) \
	AssetRegistry::Instance()->RegisterLoader<CLASS>(AssetRegistry::MakeTypeIndexVector<UNWRAP ASSETS>(), AssetRegistry::MakeExtensions(UNWRAP SUPPORTED));