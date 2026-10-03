#pragma once

#include "Asset.h"
#include "Core/Defines.h"
#include "Core/ReflectionRegistry.h"

// Call once per asset type, before registering components that reference it.
template<typename T>
void RegisterAssetReference(const String& name, const refl::TypeID& typeID)
{
    refl::ClassRegistrator<AssetRef<T>>(name, typeID)
        .Serialize([](const AssetRef<T>* ref, Archive& archive, const String& key)
        {
            archive.WriteString(ref->id.ToString(), key);
        })
        .Deserialize([](AssetRef<T>* ref, Archive& archive, const String& key)
        {
            String value;
            if (!archive.ReadString(value, key))
                return false;
            try
            {
                ref->id = AssetID(value);
                return true;
            }
            catch (...)
            {
                return false;
            }
        })
        .Finish();
}
