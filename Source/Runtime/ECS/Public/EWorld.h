#pragma once
#include <Core.h>
#include "ECommon.h"
#include "EComponentRegistry.h"
#include "EComponentAllocator.h"

class EWorld final
{

public:
	EWorld();
	~EWorld();

	EEntity CreateEntity();

	template<typename T>
	T* AddComponent(EEntity Entity, T&& Component);

	template<typename T>
	T* AddComponent(EEntity Entity);

	template<typename T>
	void RemoveComponent(EEntity Entity);

	const Vector<EEntity>& GetAllEntities() const { return mEntities; }

private:
	void ChangeArchetype(EEntity Entity, Archetype* From, Archetype* To);
private:
	UnorderedMap<EEntity, EntityLocation> mEntityToLocation;
	UnorderedMap<ESignature, Archetype*> mSigToArchetype;
	Vector<Archetype> mArchetypes;
	Vector<EEntity> mEntities;
	//Temp
	uint32 mNextEntityID = 0;
};

template<typename T>
inline T* EWorld::AddComponent(EEntity Entity, T&& Component)
{
	//Archetype* oldArchetype = mEntityToLocation[Entity].archetype;
	////Get entity component signature
	//ESignature newSignature = EComponentRegistry::Get()->CalulateSignature<T>(oldArchetype->signature);
	//
	//Archetype* newArchetype = nullptr;

	//ReflectionRegistry* reflectionRegistry = ReflectionRegistry::Get();

	////Check if archetype with new signature exist	
	//auto it = mSigToArchetype.find(newSignature);
	//if (it != mSigToArchetype.end())
	//{
	//	//if exist move entity and component to it
	//	newArchetype = it->second;
	//	ChangeArchetype(Entity, oldArchetype, newArchetype);
	//}
	//else
	//{
	//	//else create new archetype and move entity and its components
	//	Archetype archetype(newSignature);

	//	Vector<uint32> componentIds = EComponentRegistry::Get()->GetIdsFromSignature(newSignature);
	//	for (uint32 i = 0; i < componentIds.size(); i++)
	//	{
	//		uint32 id = componentIds[i];

	//		const ReflectionRegistry::TypeInfo& typeInfo = reflectionRegistry->GetInfo(EComponentRegistry::Get()->GetType(id));
	//		EComponentAllocator allocator(typeInfo, 16, 16);
	//		archetype.components[id] = std::move(allocator);
	//	}


	//	mArchetypes.push_back(std::move(archetype));
	//	newArchetype = mArchetypes.back();
	//	ChangeArchetype(Entity, oldArchetype, newArchetype);
	//}

	//void* allocatedSpace = newArchetype->components[EComponentRegistry::Get()->GetID<T>()].Allocate();
	//const ReflectionRegistry::TypeInfo& typeInfo = reflectionRegistry->GetInfo<T>();
	//typeInfo.moveFunc(allocatedSpace, &Component);

	//return reinterpret_cast<T*>(allocatedSpace);
	return nullptr;
}

template<typename T>
inline T* EWorld::AddComponent(EEntity Entity)
{
	//Archetype* oldArchetype = mEntityToLocation[Entity].archetype;
	////Get entity component signature
	//ESignature newSignature = EComponentRegistry::Get()->CalulateSignature<T>(oldArchetype->signature);

	//Archetype* newArchetype = nullptr;

	//ReflectionRegistry* reflectionRegistry = ReflectionRegistry::Get();

	////Check if archetype with new signature exist	
	//auto it = mSigToArchetype.find(newSignature);
	//if (it != mSigToArchetype.end())
	//{
	//	//if exist move entity and component to it
	//	newArchetype = it->second;
	//	ChangeArchetype(Entity, oldArchetype, newArchetype);
	//}
	//else
	//{
	//	//else create new archetype and move entity and its components
	//	Archetype archetype(newSignature);

	//	Vector<uint32> componentIds = EComponentRegistry::Get()->GetIdsFromSignature(newSignature);
	//	for (uint32 i = 0; i < componentIds.size(); i++)
	//	{
	//		uint32 id = componentIds[i];

	//		const ReflectionRegistry::TypeInfo& typeInfo = reflectionRegistry->GetInfo(EComponentRegistry::Get()->GetType(id));
	//		EComponentAllocator allocator(typeInfo, 16, 16);
	//		archetype.components[id] = std::move(allocator);
	//	}


	//	mArchetypes.push_back(std::move(archetype));
	//	newArchetype = mArchetypes.back();
	//	ChangeArchetype(Entity, oldArchetype, newArchetype);
	//}

	//void* allocatedSpace = newArchetype->components[EComponentRegistry::Get()->GetID<T>()].Allocate();
	//const ReflectionRegistry::TypeInfo& typeInfo = reflectionRegistry->GetInfo<T>();
	//typeInfo.constructFunc(allocatedSpace);

	//return reinterpret_cast<T*>(allocatedSpace);
	return nullptr;
}

template<typename T>
inline void EWorld::RemoveComponent(EEntity Entity)
{
	/*auto it = mEntityToLocation.find(Entity);
	if (it == mEntityToLocation.end())
	{
		LOG_WARNING("Trying to remove non existing component from entity, Component: {}, Entity: {}", typeid(T).name(), Entity);
		return;
	}

	auto& entityLocation = it->second;
	
	ESignature newSignature = EComponentRegistry::Get()->RemoveAndGetNew<T>(entityLocation.archetype->signature);

	Archetype* newArchetype = nullptr;
	auto sigit = mSigToArchetype.find(newSignature);
	if (sigit == mSigToArchetype.end())
	{
		Archetype archetype(newSignature);
		mArchetypes.push_back(std::move(archetype));
		newArchetype = mArchetypes.back();
	}
	else
	{
		newArchetype = sigit->second;
	}
	
	ReflectionRegistry::Get()->GetInfo<T>().destroyFunc(entityLocation.archetype->components[GetTypeId<T>()].Get(entityLocation.allocatorIndex));

	ChangeArchetype(Entity, entityLocation.archetype, newArchetype);*/
}
