#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include <Reflection/ReflectionRegistry.h>
#include "ECommon.h"
#include "Logger/Logger.h"

/*
* Engine subsystem
*/
class EComponentRegistry final 
{
	friend struct subsystem::SubsystemDescriptor;
public:
	EComponentRegistry()
	{
		//ReflectionRegistry::Get()->OnTypeRegistered.AddRaw(this, &EComponentRegistry::OnComponentRegistered);
	};

	~EComponentRegistry() = default;

	template<typename T>
	uint32 GetID()
	{
		const Reflect::ReflectTypeInfo* info = ReflectionRegistry::TryGetInfo<T>();
		if (!info)
			return UINT32_MAX;

		auto it = mUUIDToID.find(info->schema.uuid);
		if (it == mUUIDToID.end())
		{
			LOG_WARNING("Trying to get id of non-registered component: {}", info->schema.name);
			return UINT32_MAX;
		}
		return it->second;
	}

	UniqueID128 GetType(uint32 ID)
	{
		auto it = mIDToUUID.find(ID);
		if (it == mIDToUUID.end())
		{
			LOG_WARNING("Trying to get type of non-registered component ID: {}", ID);
			return {};
		}
		return it->second;
	}

	template<typename ...EComponents>
	ESignature CalulateSignature()
	{
		ESignature sig;
		(sig.set(GetID<EComponents>()), ...);
		return sig;
	}

	template<typename EComponent>
	ESignature CalulateSignature(ESignature Signature)
	{
		(Signature.set(GetID<EComponent>()));
		return Signature;
	}

	static Vector<uint32> GetIdsFromSignature(ESignature& Signature)
	{
		Vector<uint32> ids;
		for (uint32 i = 0; i < Signature.count(); i++)
		{
			ids.push_back(Signature[i]);
		}

		return ids;
	}

	ESignature CalulateSignatureFromIds(const Vector<uint32>& IDs)
	{
		ESignature sig;

		for (uint32 i = 0; i < IDs.size(); i++)
		{
			sig.set(IDs[i]);
		}

		return sig;
	}

	template<typename EComponent>
	ESignature RemoveAndGetNew(ESignature Signature)
	{
		Vector<uint32> ids = GetIdsFromSignature(Signature);
		uint32 idToRemove = GetID<EComponent>();

		ids.erase(std::remove(ids.begin(), ids.end(), idToRemove), ids.end());

		return CalulateSignatureFromIds(ids);
	}

	const String GetName(UniqueID128 Uuid)
	{
		const Reflect::ReflectTypeInfo* info = SubsystemManager::Get<ReflectionRegistry>()->TryGetInfo(Uuid);
		if (!info)
			return "";

		return String(info->schema.name);
	}

	const Vector<UniqueID128>& GetAllComponentsTypes() const { return mRegisteredComponents; }

private:
	//void OnComponentRegistered(const ReflectionRegistry::TypeInfo* Info)
	//{
	//	if (Info->flag != "Component")
	//		return;

	//	LOG_INFO("Registered Component: {}", Info->name);

	//	if (mUUIDToID.count(Info->uuid) == 0)
	//	{
	//		uint32 id = mNextID++;
	//		mUUIDToID[Info->uuid] = id;
	//		mIDToUUID[id] = Info->uuid;
	//		mRegisteredComponents.push_back(Info->uuid);
	//	}
	//}
private:
	UnorderedMap<UniqueID128, uint32> mUUIDToID;
	UnorderedMap<uint32, UniqueID128> mIDToUUID;
	Vector<UniqueID128> mRegisteredComponents;
	uint32 mNextID = 0;
};