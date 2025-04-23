#pragma once
#include <Core.h>
#include "ECommon.h"

class EWorld final
{

public:
	EWorld();
	~EWorld();

	template<typename T>
	T* AddComponent(EEntity anEntity, const T& aComponent);

	template<typename T>
	T* AddComponent(EEntity anEntity);

private:
	void ChangeArchetype(EEntity anEntity, Archetype* aFrom, Archetype* aTo);
private:
	std::unordered_map<EEntity, EntityLocation> mEntityToLocation;
	std::unordered_map<EComponentSignature, Archetype*> mSigToArchetype;
	std::vector<Archetype> mArchetypes;
};

template<typename T>
inline T* EWorld::AddComponent(EEntity anEntity, const T& aComponent)
{
	Archetype* oldArchetype = mEntityToLocation[anEntity].archetype;
	//Get entity component signature
	EComponentSignature newSignature = internal::EComponentRegistry::CalulateSignature<T>(oldArchetype->signature);
	
	Archetype* newArchetype = nullptr;

	ReflectionRegistry* reflectionRegistry = ReflectionRegistry::GetInstance();

	//Check if archetype with new signature exist	
	auto it = mSigToArchetype.find(newSignature);
	if (it != mSigToArchetype.end())
	{
		//if exist move entity and component to it
		newArchetype = it->second;
		ChangeArchetype(anEntity, oldArchetype, newArchetype);
	}
	else
	{
		//else create new archetype and move entity and its components
		Archetype archetype(newSignature);

		std::vector<EComponentID> componentIds = internal::EComponentRegistry::GetIdsFromSignature(newSignature);
		for (uint32 i = 0; i < componentIds.size(); i++)
		{
			EComponentID id = componentIds[i];

			ReflectionTypeInfo typeInfo = reflectionRegistry->GetTypeInfo(internal::EComponentRegistry::GetName(id));
			EComponentAllocator allocator(typeInfo, 16, 16);
			archetype.components[id] = allocator;
		}


		mArchetypes.push_back(archetype);
		newArchetype = mArchetypes.back();
		ChangeArchetype(anEntity, oldArchetype, newArchetype);
	}

	void* allocatedSpace = newArchetype->components[internal::EComponentRegistry::GetID<T>()].Allocate();
	ReflectionTypeInfo typeInfo = reflectionRegistry->GetTypeInfo(typeid(T));
	typeInfo.constructor(allocatedSpace, &aComponent);

	return reinterpret_cast<T*>(allocatedSpace);
}

template<typename T>
inline T* EWorld::AddComponent(EEntity anEntity)
{

	return nullptr;
}
