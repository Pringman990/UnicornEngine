#pragma once
#include <Core.h>
#include <EngineSubsystem.h>
#include <Reflection/ReflectionRegistry.h>
#include "ECommon.h"

class EComponentRegistry final : public EngineSubsystem<EComponentRegistry>
{
public:
	EComponentRegistry() = default;
	~EComponentRegistry() = default;

	template<typename T>
	void RegisterComponent()
	{
		const ReflectionRegistry::TypeInfo info = ReflectionRegistry::Get()->GetInfo<T>();

		auto it = mTypeToID.find(info.type);
		if (it != mTypeToID.end())
		{
			_LOG_CORE_WARNING("Trying to register already registered component: {}", info.name);
			return;
		}

		mTypeToID[info.type] = mNextID;
		mIDToType[mNextID] = info.type;
		mNextID++;

		_LOG_CORE_INFO("Registered Component to EComponentRegistry: {}", info.name);
	}

	template<typename T>
	uint32 GetID()
	{
		const ReflectionRegistry::TypeInfo info = ReflectionRegistry::Get()->GetInfo<T>();

		auto it = mTypeToID.find(info.type);
		if (it == mTypeToID.end())
		{
			_LOG_CORE_WARNING("Trying to get id of non-registered component: {}", info.name);
			return 0;
		}
		return it->second;
	}

	TypeIdHash GetType(uint32 ID)
	{
		auto it = mIDToType.find(ID);
		if (it == mIDToType.end())
		{
			_LOG_CORE_WARNING("Trying to get type of non-registered component ID: {}", ID);
			return 0;
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

	const String GetName(TypeIdHash Type)
	{
		return String(ReflectionRegistry::Get()->GetInfo(Type).name);
	}

private:
	UnorderedMap<TypeIdHash, uint32> mTypeToID;
	UnorderedMap<uint32, TypeIdHash> mIDToType;
	uint32 mNextID = 0;
};