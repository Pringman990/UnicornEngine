#include "Input/Assets/InputMappingLoader.h"

InputMappingLoader::InputMappingLoader()
{
}

InputMappingLoader::~InputMappingLoader()
{
}

InputMapping* InputMappingLoader::CreateEmptyAsset(UniqueID128 UUID)
{
	return new InputMapping(UUID);
}

bool InputMappingLoader::Load(AssetBase* Asset, const String& VirtualPath)
{
	//AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	//if (!readData.UUID.IsValid())
	//{
	//	LOG_ERROR("Trying to load asset without uuid");
	//	return nullptr;
	//}

	//InputMapping* asset = new InputMapping(readData.UUID);
	//asset->SetMetaPath(VirtualPath);
	//asset->SetName(readData.Name);
	//asset->SetType(readData.Type);

	InputMapping* asset = static_cast<InputMapping*>(Asset);
	AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	Archive& arc = *readData.CustomData.get();

	arc.BeginReadObject("bindings");

	String actionUUIDStr;
	while (arc.ReadObjectKey(actionUUIDStr))
	{
		UniqueID128 actionUUID(actionUUIDStr);

		arc.BeginReadArray(actionUUIDStr);
		uint32 actionsArrSize = arc.GetArraySize();
		for (uint32 i = 0; i < actionsArrSize; i++)
		{
			arc.BeginReadObject();

			InputBinding binding;
			arc.ReadString(binding.key, "key");

			arc.BeginReadArray("modifiers");
			uint32 modifiersSize = arc.GetArraySize();
			for (uint32 j = 0; j < modifiersSize; j++)
			{
				uint8 modifier;
				arc.ReadUInt8(modifier, "modifier");
				binding.modifiers.push_back((InputBinding::Modifier)modifier);
				readData.CustomData->Next();
			}
			arc.EndReadArray();

			arc.EndReadObject();

			AssetRef<InputAction> actionAsset = AssetRegistry::Instance()->Load<InputAction>(actionUUID);

			if (actionAsset)
				asset->AddBinding(actionAsset, std::move(binding));
			else
				LOG_ERROR("Failed to get input action from uuid '{}'", actionUUIDStr);

			arc.Next();
		}
		arc.EndReadArray();
	}
	arc.EndReadObject();
	return asset;
}

InputMapping* InputMappingLoader::ImportSource(const String& VirtualSourcePath)
{
	THROW("Input mapping has no source");
	return nullptr;
}

InputMapping* InputMappingLoader::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
	THROW("Not Implemeted");
	return nullptr;
}

bool InputMappingLoader::UnLoad(const String& VirtualPath)
{
	THROW("Not Implemeted");
	return false;
}
