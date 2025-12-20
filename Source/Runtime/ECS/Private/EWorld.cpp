#include "EWorld.h"

EWorld::EWorld()
{
}

EWorld::~EWorld()
{
}

EEntity EWorld::CreateEntity()
{
	EEntity ent = UniqueID128::FromRandom();
	mEntities.push_back(ent);
	return ent;
}

EEntity EWorld::CreateEntity(const UniqueID128& UUID)
{
	EEntity ent = UUID;
	mEntities.push_back(ent);
	return ent;
}

void* EWorld::AddComponent(EEntity Entity, const UniqueID128& UUID)
{
	const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull(UUID);
	if (!type)
	{
		LOG_WARNING("Can't get ComponentStore with type uuid '{}'", UUID.ToString());
		return nullptr;
	}

	auto it = mComponentStores.find(type->uuid);
	if (it == mComponentStores.end())
	{
		ComponentStore store = {};
		store.allocator = EComponentAllocator(type, 10, 10);
		mComponentStores.emplace(type->uuid, std::move(store));
	}

	auto& store = mComponentStores[type->uuid];
	void* component = store.allocator.Allocate();
	store.entities.push_back(Entity);
	uint32 entityIndex = store.allocator.GetCount() - 1;
	store.indicies[Entity] = entityIndex;
	store.indexToEntity[entityIndex] = Entity;

	return component;
}

void EWorld::RemoveComponent(EEntity Entity, const UniqueID128& UUID)
{
	const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull(UUID);
	if (!type)
	{
		LOG_WARNING("Can't get ComponentStore with type uuid '{}'", UUID.ToString());
		return;
	}

	auto it = mComponentStores.find(type->uuid);
	if (it == mComponentStores.end())
	{
		LOG_ERROR("Can't remove component that isn't registered, {}", type->name);
		return;
	}

	auto& store = mComponentStores[type->uuid];

	auto entityIt = store.indicies.find(Entity);
	if (entityIt == store.indicies.end())
	{
		LOG_ERROR("Can't remove component that the entity doesn't have");
		return;
	}

	store.allocator.RemoveAt(entityIt->second);

	store.entities[entityIt->second] = store.entities.back();
	store.entities[entityIt->second] = entityIt->first;

	store.indexToEntity[entityIt->second] = store.entities[entityIt->second];
	store.indexToEntity.erase(store.allocator.GetCount());

	store.indicies.erase(Entity);
	store.entities.pop_back();
}

//void EWorld::ChangeArchetype(EEntity Entity, Archetype* From, Archetype* To)
//{
//	//EntityLocation& entityLocation = mEntityToLocation[Entity];
//	//uint32 entityIndex = entityLocation.allocatorIndex;
//	//
//	//EEntity entityToSwapWith = From->entities.back();
//	//uint32 entityToSwapWithIndex = mEntityToLocation[entityToSwapWith].allocatorIndex;
//	//mEntityToLocation[entityToSwapWith].allocatorIndex = entityIndex;
//	//
//	//std::swap(From->entities.back(), From->entities[entityIndex]);
//	//From->entities.pop_back();
//
//	//for (auto& [typeHash, componentArray] : From->components)
//	//{
//	//	componentArray.Swap(entityIndex, entityToSwapWithIndex);
//
//	//	void* newBlock = To->components[typeHash].Allocate();
//	//	const auto& typeInfo = ReflectionRegistry::Get()->GetInfo(typeHash);
//	//	typeInfo.moveFunc(newBlock, componentArray.Back());
//
//	//	componentArray.Pop();
//	//}
//
//	//To->entities.push_back(Entity);
//	//entityLocation.allocatorIndex = static_cast<uint32>(To->entities.size() - 1);
//	//entityLocation.archetype = To;
//}
