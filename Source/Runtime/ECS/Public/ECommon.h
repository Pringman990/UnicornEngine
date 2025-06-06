#pragma once
#include <Core.h>
#include "EComponentAllocator.h"

class EWorld;
#undef max

using EEntity = uint32;

constexpr size_t MAX_COMPONENTS = 256;
using ESignature = std::bitset<MAX_COMPONENTS>;

using EComponentID = size_t;

using EComponentSignature = ESignature;
using EComponentType = std::type_index;

template<typename... EComponents>
using ESystemFunctionT = Func<void(EWorld&, EEntity, EComponents&...)>;

using ESystemFunction = Func<void(EWorld&, EEntity)>;

enum EPipeline
{
	ELoad,
	EPostLoad,
	EPreUpdate,
	EUpdate,
	EPostUpdate,
	ERender,
	ECount
};

struct ESystem final
{
	EPipeline pipeline = EUpdate;
	String name = "";
	ESignature signature = 0;
	ESystemFunction function;
};

using EPipelineSystemMap = UnorderedMap<EPipeline, Vector<ESystem>>;
using ENameSystemMap = UnorderedMap<String, ESystem>;

struct Archetype
{
	Archetype(EComponentSignature Signature) : signature(Signature) {};

	const EComponentSignature signature;
	UnorderedMap<EComponentID, EComponentAllocator> components;
	Vector<EEntity> entities;

};

struct EntityLocation
{
	Archetype* archetype;
	uint32 indexInArray;
};

namespace internal
{
	class EComponentRegistry final
	{
	public:
		template<typename T>
		static void RegisterComponent()
		{
			static EComponentID nextID = 1;

			ReflectionTypeInfo info = ReflectionRegistry::Get()->GetTypeInfo(typeid(T));

			auto it = sNameToID.find(info.name);
			if (it != sNameToID.end())
			{
				_LOG_CORE_WARNING("Trying to register already registered component: {}", info.name);
				return;
			}

			sNameToID[info.name] = nextID;
			sIDToName[nextID] = info.name;
			nextID++;

			_LOG_CORE_INFO("Registered Component to EComponentRegistry: {}", info.name);
		}

		template<typename T>
		static EComponentID GetID()
		{
			ReflectionTypeInfo info = ReflectionRegistry::Get()->GetTypeInfo(typeid(T));

			auto it = sNameToID.find(info.name);
			if (it == sNameToID.end())
			{
				_LOG_CORE_WARNING("Trying to get id of non-registered component: {}", info.name);
				return 0;
			}
			return it->second;
		}

		template<typename ...EComponents>
		static EComponentSignature CalulateSignature()
		{
			EComponentSignature sig;
			(sig.set(GetID<EComponents>()), ...);
			return sig;
		}

		template<typename EComponent>
		static EComponentSignature CalulateSignature(EComponentSignature Signature)
		{
			(Signature.set(GetID<EComponent>()));
			return Signature;
		}

		static Vector<EComponentID> GetIdsFromSignature(EComponentSignature& Signature)
		{
			Vector<EComponentID> ids;
			for (uint32 i = 0; i < Signature.count(); i++)
			{
				ids.push_back(Signature[i]);
			}

			return ids;
		}

		static const String& GetName(EComponentID Id)
		{
			return sIDToName[Id];
		}

	private:
		inline static UnorderedMap<String, EComponentID> sNameToID;
		inline static UnorderedMap<EComponentID, String> sIDToName;
	};
}
