#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>

#include "IAssetManager.h"
#include "ResourceManagment/HeapResourcePool.h"

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
	AssetHandle<T> handle;
	AssetLoadResult result = AssetLoadResult::Failed;
};

struct AssetFileReadData
{
	String Type;
	UniqueID128 UUID;
	YAML::Node CustomData;
	String SourcePath;
};

/*
* Engine subsystem
*/
class AssetRegistry
{
	friend struct subsystem::SubsystemDescriptor;
public:

	template<typename T>
	void RegisterManager(const Vector<String>& Extensions)
	{
		std::type_index id = typeid(T);
		auto it = mManagers.find(id);
		if (it != mManagers.end())
		{
			LOG_WARNING("Asset Manager is already registered, type id name: {}", id.name());
			return;
		}

		mManagers[id] = MakeOwned<T>();

		for (auto& extension : Extensions)
		{
#ifdef _DEBUG
			auto it = mExtensionToManager.find(extension);
			if (it != mExtensionToManager.end())
			{
				THROW("Extension already has registered manager: {}", extension);
			}
#endif // _DEBUG

			mExtensionToManager[extension] = mManagers[id].get();
		}
	}

	template<typename T>
	T* GetManager()
	{
		std::type_index id = typeid(T);
		auto it = mManagers.find(id);
		if (it == mManagers.end())
		{
			LOG_WARNING("Asset Manager was not found in registry, type id name: {}", id.name());
			return nullptr;
		}

		return static_cast<T*>(it->second.get());
	}

	template<typename T>
	T* GetManagerFromAssetType()
	{
		std::type_index id = typeid(T);
		auto it = mAssetTypeToManager.find(id);
		if (it == mAssetTypeToManager.end())
		{
			LOG_WARNING("Asset Manager was not found in registry, type id name: {}", id.name());
			return nullptr;
		}

		return static_cast<T*>(it->second);
	}

	/**
	* This function can load asset files and source files.
	* It will decied on load method depending on file type.
	* If asset with source file already exist when loading source it will try to reimport the source instead of creating a new asset.
	*/
	template<typename T>
	AssetLoadData<T> Load(const String& VirtualPath)
	{
		if (GET_FILESYSTEM()->Exists(VirtualPath) == false)
		{
			LOG_WARNING("Trying to load a asset that does not exist, path: {}", VirtualPath);
			return {};
		}

		String extension = ExtractExtension(VirtualPath);
		if (extension == "")
		{
			LOG_WARNING("Path with no extension was entered, path: {}", VirtualPath);
			return {};
		}

		if (extension == "asset")
		{
			std::type_index id = typeid(T);
			auto it = mAssetTypeToManager.find(id);
			if (it == mAssetTypeToManager.end())
			{
				LOG_WARNING("Asset Manager was not found in registry when trying to load asset path: {}, type id name: {}", VirtualPath, id.name());
				return {};
			}

			AssetLoadData<T> data;
			data.handle = ToResourceHandle<T>(it->second->Load(VirtualPath));
			data.result = AssetLoadResult::AssetLoaded;
			return data;
		}

		auto it = mExtensionToManager.find(extension);
		if (it == mExtensionToManager.end())
		{
			LOG_WARNING("No asset manager that supports extension ({}) was found for path: {}", extension, VirtualPath);
			return {};
		}

		String potensialAssetPath = ExtractPathWithoutExtension(VirtualPath);
		potensialAssetPath += ".asset";

		UniqueID128 uuid = it->second->FindAssetBySource(VirtualPath);
		if (uuid.IsValid())
		{
			AssetLoadData<T> data;
			data.handle = ToResourceHandle<T>(it->second->ReimportSource(VirtualPath, uuid));
			data.result = AssetLoadResult::SourceReimported;
			return data;
		}
		else if(GET_FILESYSTEM()->Exists(potensialAssetPath))
		{
			std::type_index id = typeid(T);
			auto it = mAssetTypeToManager.find(id);
			if (it == mAssetTypeToManager.end())
			{
				LOG_WARNING("Asset Manager was not found in registry when trying to load asset path: {}, type id name: {}", VirtualPath, id.name());
				return {};
			}

			AssetLoadData<T> data;
			data.handle = ToResourceHandle<T>(it->second->Load(potensialAssetPath));
			data.result = AssetLoadResult::AssetLoaded;
			return data;
		}
		else
		{
			AssetLoadData<T> data;
			data.handle = ToResourceHandle<T>(it->second->ImportSource(VirtualPath));
			data.result = AssetLoadResult::SourceImported;
			return data;
		}
	}

	/**
	* Creates a YAML asset file at VirtualPath location.
	* SourcePath can be used if the asset has external data like mesh asset files has fbx files as source.
	*/
	ENGINE_API static void CreateAssetFile(
		const String& VirtualPath,
		const String& Type,
		const UniqueID128& UUID,
		const YAML::Node CustomData = {},
		const String& SourcePath = ""
	);

	ENGINE_API static AssetFileReadData ReadAssetFile(const String& VirtualPath);

	/**
	* Used to register managers during static initilization.
	*
	* @note
	* Don't call manually, use REGISTER_ASSET_MANAGER in the cpp of the manager you want to register.
	*/
	ENGINE_API static void DefferRegistration(
		std::type_index Type, 
		OwnedPtr<IAssetManager> Instance, 
		const Vector<std::type_index>& AssetTypes,
		const Vector<String>& Extensions
	);

	ENGINE_API void RegisterAllDefferedManagers();

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

private:
	UnorderedMap<std::type_index, OwnedPtr<IAssetManager>> mManagers;
	UnorderedMap<String, IAssetManager*> mExtensionToManager;
	UnorderedMap<std::type_index, IAssetManager*> mAssetTypeToManager;
};

#define GET_ASSETREGISTRY() SubsystemManager::Get<AssetRegistry>()

#define UNWRAP(...) __VA_ARGS__

/**
* The args passed will be the registered extensions for the manager.
*
* @ingroup
* AssetRegistry.
*/
#define REGISTER_ASSET_MANAGER(CLASS, ASSETS, SUPPORTED) \
namespace { \
	struct __Asset_Manager_Auto_Register { \
		__Asset_Manager_Auto_Register() { \
			AssetRegistry::DefferRegistration(typeid(CLASS), MakeOwned<CLASS>(), AssetRegistry::MakeTypeIndexVector<UNWRAP ASSETS>(), AssetRegistry::MakeExtensions(UNWRAP SUPPORTED)); \
		} \
	}; \
	static const __Asset_Manager_Auto_Register TRUNCATE(__Asset_Manager_Auto_Register, __LINE__); \
}