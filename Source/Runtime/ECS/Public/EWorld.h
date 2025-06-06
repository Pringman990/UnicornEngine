#pragma once
#include <Core.h>
#include "ECommon.h"

class EWorld final
{

public:
	EWorld();
	~EWorld();

	template<typename T>
	T* AddComponent(EEntity Entity, const T& Component);

	template<typename T>
	T* AddComponent(EEntity Entity);

private:
	void ChangeArchetype(EEntity Entity, Archetype* From, Archetype* To);
private:
	UnorderedMap<EEntity, EntityLocation> mEntityToLocation;
	UnorderedMap<EComponentSignature, Archetype*> mSigToArchetype;
	Vector<Archetype> mArchetypes;
};

template<typename T>
inline T* EWorld::AddComponent(EEntity Entity, const T& Component)
{
	Archetype* oldArchetype = mEntityToLocation[Entity].archetype;
	//Get entity component signature
	EComponentSignature newSignature = internal::EComponentRegistry::CalulateSignature<T>(oldArchetype->signature);
	
	Archetype* newArchetype = nullptr;

	ReflectionRegistry* reflectionRegistry = ReflectionRegistry::Get();

	//Check if archetype with new signature exist	
	auto it = mSigToArchetype.find(newSignature);
	if (it != mSigToArchetype.end())
	{
		//if exist move entity and component to it
		newArchetype = it->second;
		ChangeArchetype(Entity, oldArchetype, newArchetype);
	}
	else
	{
		//else create new archetype and move entity and its components
		Archetype archetype(newSignature);

		Vector<EComponentID> componentIds = internal::EComponentRegistry::GetIdsFromSignature(newSignature);
		for (uint32 i = 0; i < componentIds.size(); i++)
		{
			EComponentID id = componentIds[i];

			ReflectionTypeInfo typeInfo = reflectionRegistry->GetTypeInfo(internal::EComponentRegistry::GetName(id));
			EComponentAllocator allocator(typeInfo, 16, 16);
			archetype.components[id] = allocator;
		}


		mArchetypes.push_back(archetype);
		newArchetype = mArchetypes.back();
		ChangeArchetype(Entity, oldArchetype, newArchetype);
	}

	void* allocatedSpace = newArchetype->components[internal::EComponentRegistry::GetID<T>()].Allocate();
	ReflectionTypeInfo typeInfo = reflectionRegistry->GetTypeInfo(typeid(T));
	typeInfo.constructor(allocatedSpace, &Component);

	return reinterpret_cast<T*>(allocatedSpace);
}

template<typename T>
inline T* EWorld::AddComponent(EEntity Entity)
{

	return nullptr;
}
