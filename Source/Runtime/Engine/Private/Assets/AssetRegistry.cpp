#include "Assets/AssetRegistry.h"

#include "YAML/YamlHelper.h"
#include "FileSystem/FileSystem.h"

REGISTER_ENGINE_SUBSYSTEM(AssetRegistry)

namespace
{
	struct DefferedInfo
	{
		OwnedPtr<IAssetManager> instance;
		Vector<std::type_index> assetTypes;
		Vector<String> extensions;
	};

	ENGINE_API UnorderedMap<std::type_index, DefferedInfo>& GetDefferedAssetManagerRegistrations()
	{
		static UnorderedMap<std::type_index, DefferedInfo> managers;
		return managers;
	}
}

ENGINE_API void AssetRegistry::DefferRegistration(std::type_index Type, OwnedPtr<IAssetManager> Instance, const Vector<std::type_index>& AssetTypes, const Vector<String>& Extensions)
{
	ASSERT(GetDefferedAssetManagerRegistrations().contains(Type) == false, "Asset manager has already been registered");

	GetDefferedAssetManagerRegistrations().insert({ Type, DefferedInfo{std::move(Instance), AssetTypes, Extensions} });
}

AssetRegistry::AssetRegistry()
{
}

AssetRegistry::~AssetRegistry()
{
}

ENGINE_API void AssetRegistry::CreateAssetFile(
	const String& VirtualPath, 
	const String& Type, 
	const UniqueID128& UUID,
	const YAML::Node CustomData,
	const String& SourcePath
)
{
	_PAUSE_TRACK_MEMORY(true);
	YAML::Node root;

	root["uuid"] = UUID.ToString();
	root["type"] = Type;
	
	if (SourcePath != "")
	{
		root["source_path"] = SourcePath;
	}
	
	if (!CustomData.IsNull())
	{
		root["custom_data"] = CustomData;
	}

	ByteBuffer buffer = YamlHelper::WriteToMemory(root);

	GET_FILESYSTEM()->WriteAll(VirtualPath, buffer);
	_PAUSE_TRACK_MEMORY(false);
}

ENGINE_API AssetFileReadData AssetRegistry::ReadAssetFile(const String& VirtualPath)
{
	ByteBuffer buffer = GET_FILESYSTEM()->ReadAll(VirtualPath);
	YAML::Node root = YamlHelper::LoadFromMemory(buffer);

	AssetFileReadData readData;
	readData.UUID = UniqueID128(root["uuid"].as<String>());
	readData.Type = root["type"].as<String>();
	readData.SourcePath = root["source_path"].as<String>();
	readData.CustomData = root["custom_data"];

	root.reset();
	return readData;
}

ENGINE_API void AssetRegistry::RegisterAllDefferedManagers()
{
	for (auto& [managerType, defferedInfo] : GetDefferedAssetManagerRegistrations())
	{
		auto& manager = defferedInfo.instance;
		mManagers.insert({ managerType, std::move(manager) });
		
		auto& ext = defferedInfo.extensions;
		for (auto& extension : ext)
		{
#ifdef _DEBUG
			auto it = mExtensionToManager.find(extension);
			if (it != mExtensionToManager.end())
			{
				THROW("Extension already has registered manager: {}", extension);
			}
#endif // _DEBUG

			mExtensionToManager[extension] = mManagers[managerType].get();
		}

		auto& assetType = defferedInfo.assetTypes;
		for (auto& type : assetType)
		{
#ifdef _DEBUG
			auto it = mAssetTypeToManager.find(type);
			if (it != mAssetTypeToManager.end())
			{
				THROW("Asset type already has registered manager: {}", type.name());
			}
#endif // _DEBUG

			mAssetTypeToManager[type] = mManagers[managerType].get();
		}
	}

	GetDefferedAssetManagerRegistrations().clear();
}