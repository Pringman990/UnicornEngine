#include "EnginePch.h"
#include "ECSWorld.h"

ecs::World::World()
{
}

ecs::World::~World()
{
}

ecs::Entity ecs::World::CreateEntity()
{
	Entity entity = mNextEntity++;
	//mEntityComponentMasks[entity] = 0;
	return entity;
}

void ecs::World::ProcessSystems()
{
	for (Pipeline pipeline = 0; pipeline < PIPELINE_COUNT; pipeline++)
	{
		for (auto& system : mSystems[pipeline]) 
		{
				system(*this, );
		}
	}
}

template<typename T>
void ecs::World::AddComponent(Entity anEntity, T aComponent)
{

}

template<typename ...Components>
void ecs::World::BindSystem(System<Components...> aSystem, Pipeline aPipeline)
{
	mSystems[aPipeline].push_back(
		[=](World& world, Entity entity) 
		{

			aSystem(world, entity, )
		});
}



//template<typename ...Components>
//void ecs::World::BindSystem(sys<Components...> aSystem, Pipeline aPipeline)
//{
//
//}


//template<typename ...Components>
//void ecs::World::BindSystem(Pipeline aPipeline)
//{
//
//}




//template<typename T>
//void ecs::World::AddComponent(Entity anEntity, T aComponent)
//{
//	auto it = mEntityComponentMasks.find(anEntity);
//	if (it == mEntityComponentMasks.end())
//		return;
//
//	auto& mask = mEntityComponentMasks[anEntity];
//	mask |= internal::ComponentMaskRegistry::GetInstance().TryGetMask<T>();
//	GetComponentStorage<T>()->data.push_back(aComponent);
//	FindNewArchetypeForEntity(anEntity, mask);
//}
//
//template<typename T>
//void ecs::World::RemoveComponent(Entity anEntity)
//{
//	auto it = mEntityComponentMasks.find(anEntity);
//	if (it == mEntityComponentMasks.end())
//		return;
//
//	auto& mask = mEntityComponentMasks[anEntity];
//	mask &= ~internal::ComponentMaskRegistry::GetInstance().TryGetMask<T>();
//	GetComponentStorage<T>()->data.erase(entity);
//	FindNewArchetypeForEntity(anEntity, mask);
//}
//
//template<typename T>
//bool ecs::World::HasComponent(Entity anEntity)
//{
//	auto it = mEntityComponentMasks.find(anEntity);
//	if (it == mEntityComponentMasks.end())
//		return false;
//
//	return (mEntityComponentMasks[anEntity] & internal::ComponentMaskRegistry::GetInstance().TryGetMask<T>()) != 0;
//}
//
//template<typename T>
//T ecs::World::GetComponent(Entity anEntity)
//{
//	mEntityComponentMasks[anEntity];
//	return T();
//}
//
//template<typename ...Components>
//void ecs::World::BindSystem(SystemFunction aSystem, Pipeline aPipeline)
//{
//	ArchetypeMask mask = (internal::ComponentMaskRegistry::GetInstance().TryGetMask<Components>() | ...);
//	mSystems[aPipeline].push_back({ mask, aSystem });
//}
//
//void ecs::World::ProcessSystems()
//{
//	Pipeline currentPipeline = OnPreUpdate;
//
//	for (const auto& [mask, system] : mSystems[currentPipeline])
//	{
//		if (currentPipeline > OnPostUpdate)
//			break;
//
//		auto it = mArchetypes.find(mask);
//		if (it != mArchetypes.end()) {
//			for (auto entity : it->second) 
//			{
//				auto components = GetComponentsForEntity(entity);
//
//				// Call the system function with the components
//				std::apply(system, std::tuple_cat(std::make_tuple(*this, entity), components));
//			}
//		}
//
//		currentPipeline++;
//	}
//}
//
//template<typename... Components>
//std::tuple<Components&...> ecs::World::GetComponentsForEntity(Entity entity)
//{
//	return std::make_tuple(GetComponent<Components>(entity)...);
//}
//
//bool ecs::World::EntityMatchesArchetype(Entity anEntity, ArchetypeMask aMask)
//{
//	auto it = mEntityComponentMasks.find(anEntity);
//	if (it == mEntityComponentMasks.end())
//		return false;
//
//	return (mEntityComponentMasks[anEntity] & aMask) == aMask;
//}
//
//void ecs::World::FindNewArchetypeForEntity(Entity anEntity, ArchetypeMask aMask)
//{
//	auto it = mEntityComponentMasks.find(anEntity);
//	if (it == mEntityComponentMasks.end())
//		return;
//
//	mEntityComponentMasks[anEntity] = aMask;
//	for (auto& archetype : mArchetypes)
//	{
//		auto& entities = archetype.second;
//		if (EntityMatchesArchetype(anEntity, archetype.first))
//		{
//			entities.insert(anEntity);
//		}
//		else
//		{
//			entities.erase(anEntity);
//		}
//	}
//}
//
//template<typename T>
//ecs::ComponentStorage<T>* ecs::World::GetComponentStorage()
//{
//	auto it = mRegisteredComponents.find(typeid(T));
//	if (it == mRegisteredComponents.end())
//	{
//		auto storage = new ComponentStorage<T>();
//		mRegisteredComponents[typeid(T)] = storage;
//		return storage;
//	}
//
//	return static_cast<ComponentStorage<T>*>(it->second);
//}