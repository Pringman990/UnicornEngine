#pragma once
#include <Assets/AssetRegistry.h>
#include "AssetRef.h"

template<typename T>
T* AssetRef<T>::Get() const
{
	return AssetRegistry::Instance()->GetAsset(*this);
}

template<IsAsset T>
inline void SaveAssetRef(void* obj, Archive& archive, const String& key)
{
	AssetRef<T>& ref = *static_cast<AssetRef<T>*>(obj);
	
	String uuid;
	if (ref)
		uuid = ref.Get()->GetUUID().ToString();
	else
		uuid = UniqueID128::Invalid().ToString();
	
	archive.WriteString(uuid, key);
}

template<IsAsset T>
inline void LoadAssetRef(void* obj, Archive& archive, const String& key)
{
	AssetRef<T>* ref = static_cast<AssetRef<T>*>(obj);
	String uuidStr;
	archive.ReadString(uuidStr, key);

	(*ref) = AssetRegistry::Instance()->template GetAssetFromUUID<T>(UniqueID128(uuidStr));
}