#include "Input/Assets/InputActionLoader.h"

InputActionLoader::InputActionLoader()
{
}

InputActionLoader::~InputActionLoader()
{
}

InputAction* InputActionLoader::CreateEmptyAsset(UniqueID128 UUID)
{
	return new InputAction(UUID);
}

bool InputActionLoader::Load(AssetBase* Asset, const String& VirtualPath)
{
	//AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	//if (!readData.UUID.IsValid())
	//{
	//	LOG_ERROR("Trying to load asset without uuid");
	//	return nullptr;
	//}

	//InputAction* asset = new InputAction(readData.UUID);
	//asset->SetMetaPath(VirtualPath);
	//asset->SetName(readData.Name);
	//asset->SetType(readData.Type);

	InputAction* asset = static_cast<InputAction*>(Asset);
	AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	uint8 type;
	readData.CustomData->ReadUInt8(type, "type");
	asset->SetActionType((InputActionType)type);

	uint8 trigger;
	readData.CustomData->ReadUInt8(trigger, "trigger");
	asset->SetActionTrigger((InputActionTrigger)trigger);

	return asset;
}

InputAction* InputActionLoader::ImportSource(const String& VirtualSourcePath)
{
	return nullptr;
}

InputAction* InputActionLoader::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
	return nullptr;
}

bool InputActionLoader::UnLoad(const String& VirtualPath)
{
	return false;
}
