#pragma once
#include <Core.h>
#include <EngineDefines.h>
#include "Reflection/ReflectionRegistry.h"

class AssetRegistry;
class SubsystemManager;
class AssetBase;

struct AssetRefHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	static void Invalidate(AssetRefHandle& Handle)
	{
		Handle.index = UINT32_MAX;
		Handle.generation = 0;
	};

	inline static AssetRefHandle MakeInvalid() { return { UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && generation != 0;
	}

	bool operator==(const AssetRefHandle& other) const = default;
};

template<typename T>
concept IsAsset = std::is_base_of<AssetBase, T>::value;

template<typename T>
struct AssetRef : AssetRefHandle
{
	AssetRef()
	{
		index = UINT32_MAX;
		generation = 0;
	}

	AssetRef(AssetRefHandle Handle)
	{
		index = Handle.index;
		generation = Handle.generation;
	}

	inline static AssetRef<T> Invalid() { return AssetRef<T>(AssetRefHandle::MakeInvalid()); };

	T* Get() const;

	T* operator->() const noexcept
	{
		return Get();
	}

	operator bool() const
	{
		return AssetRefHandle::operator bool() && Get() != nullptr;
	}
};

template<IsAsset T>
inline void SaveAssetRef(void* obj, Archive& archive, const String& key);

template<IsAsset T>
inline void LoadAssetRef(void* obj, Archive& archive, const String& key);

#include "Assets/AssetRef.inl"