#include "Assets/AssetRegistry.h"

#include "YAML/YamlHelper.h"
#include "FileSystem/FileSystem.h"
#include "Archive/YamlArchive.h"

#include <filesystem>

REGISTER_ENGINE_SUBSYSTEM(AssetRegistry)

AssetRegistry::AssetRegistry()
{
}

AssetRegistry::~AssetRegistry()
{
}

void AssetRegistry::FindAndRegisterAllAssets()
{
	Path root = GET_FILESYSTEM()->GetAbsolutPath("engine://");
	for (auto& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (!entry.is_directory() && entry.path().extension() == ".asset")
		{
			auto relative = std::filesystem::relative(entry.path(), root);
			Path virtualPath = "engine://" + relative.generic_string();

			ByteBuffer buffer = GET_FILESYSTEM()->ReadAll(virtualPath);
			YamlArchive arc(buffer);

			String type;
			arc.ReadString(type, "type");
			Load(virtualPath, type);
		}
	}
}

ENGINE_API void AssetRegistry::CreateAssetFile(
	const String& VirtualPath, 
	const String& Type, 
	const String& Name, 
	const UniqueID128& UUID,
	const YAML::Node CustomData,
	const String& SourcePath
)
{
	_PAUSE_TRACK_MEMORY(true);

	YamlArchive arc;

	arc.WriteString(UUID.ToString(), "uuid");
	arc.WriteString(Type, "type");
	arc.WriteString(Name, "name");
	
	if (SourcePath != "")
	{
		arc.WriteString(SourcePath, "source_path");
	}

	arc.WriteToFile(VirtualPath);
	_PAUSE_TRACK_MEMORY(false);
}

ENGINE_API AssetFileReadData AssetRegistry::ReadAssetFile(const String& VirtualPath)
{
	if (!GET_FILESYSTEM()->Exists(VirtualPath))
	{
		LOG_WARNING("Trying to read a asset that doesn't exist, {}", VirtualPath);
		return {};
	}

	ByteBuffer buffer = GET_FILESYSTEM()->ReadAll(VirtualPath);

	YamlArchive arc(buffer);

	AssetFileReadData readData;

	String uuidStr;
	arc.ReadString(uuidStr, "uuid");
	readData.UUID = UniqueID128(uuidStr);

	arc.ReadString(readData.Type, "type");
	arc.ReadString(readData.Name, "name");

	if(arc.HasKey("source_path"))
		arc.ReadString(readData.SourcePath, "source_path");

	if (arc.HasKey("custom_data"))
	{
		readData.CustomData = std::move(arc.CreateSubArchive("custom_data"));
	}

	return readData;
}