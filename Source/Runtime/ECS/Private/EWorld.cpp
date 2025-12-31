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

	if (HasComponent(Entity, UUID))
	{
		LOG_WARNING("Trying to add already existing component on entity '{}'", Entity.ToString());
		return nullptr;
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

	EEntity ent = entityIt->first;
	uint32 index = entityIt->second;
	EEntity lastEntity = store.entities.back();
	uint32 lastIndex = store.indicies[lastEntity];

	store.allocator.RemoveAt(index);

	if (index == lastIndex)
	{
		store.entities.pop_back();
		store.indexToEntity.erase(store.allocator.GetCount());
		store.indicies.erase(Entity);
	}
	else
	{
		store.entities[index] = store.entities.back();
		store.entities.pop_back();

		store.indexToEntity[index] = store.entities[index];
		store.indexToEntity.erase(store.allocator.GetCount());

		store.indicies.erase(Entity);
		store.indicies[store.indexToEntity[index]] = index;
	}
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

bool EWorld::HasComponent(EEntity Entity, UniqueID128 UUID)
{
	auto it = mComponentStores.find(UUID);
	if (it == mComponentStores.end())
	{
		return false;
	}

	return it->second.indicies.contains(Entity);
}

ECS_API UnorderedMap<UniqueID128, void*> EWorld::GetAllComponentsOnEntity(EEntity Entity)
{
	UnorderedMap<UniqueID128, void*> map;
	for (auto& [typeUUID, store] : mComponentStores)
	{
		auto it = std::find(store.entities.begin(), store.entities.end(), Entity);
		if (it == store.entities.end())
			continue;

		map.insert({ typeUUID, store.allocator.Get(store.indicies[Entity]) });
	}

	return map;
}