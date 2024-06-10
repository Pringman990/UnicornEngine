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
	auto it = mArchetypes.find(0);
	if (it != mArchetypes.end())
	{
		it->second.AddEntity(entity);
	}
	//mEntityComponentMasks[entity] = 0;
	return entity;
}

void ecs::World::ProcessSystems()
{
	for (Pipeline pipeline = OnPreUpdate; pipeline < PIPELINE_COUNT; pipeline++)
	{
		for (auto& [signature, system] : mSystems[pipeline])
		{
			auto it = mSignatureToArchetypes.find(signature);
			if (it != mSignatureToArchetypes.end())
			{
				for (Archetype* archetype : it->second)
				{
					for (auto& entity : archetype->entityComponents)
					{
						system(*this, entity.first, *archetype);
					}
				}
			}
		}
	}
}

void ecs::World::PreComputeSignatureToArchetype()
{
	mSignatureToArchetypes.clear();
	for (auto& [signature, archetype] : mArchetypes)
	{
		for (Signature subset = signature; subset > 0; subset = (subset - 1) & signature)
		{
			mSignatureToArchetypes[subset].push_back(&archetype);
		}
	}
}

template<typename ...Components>
void ecs::World::BindSystem(System<Components...> aSystem, Pipeline aPipeline)
{
	Signature signature = CalulateSignature<Components...>();
	mSystems[aPipeline].emplace_back(signature,
		[=](World& world, Entity entity, Archetype& archetype)
		{
			aSystem(world, entity, archetype.GetComponent<Components>(entity)...)
		});
}

template<typename ...Components>
ecs::Signature ecs::World::CalulateSignature()
{
	Signature signature = 0;
	((signature |= (1 << internal::ComponentRegistry::GetInstance().TryGetMask<Components>())), ...);
	return signature;
}

template<typename T>
void ecs::World::AddComponent(Entity anEntity, T aComponent)
{

}

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