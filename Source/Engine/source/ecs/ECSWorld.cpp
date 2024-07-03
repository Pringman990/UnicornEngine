#include "EnginePch.h"
#include "ECSWorld.h"

#include "SystemManager.h"

ecs::World::World()
	:
	mNextEntity(1)
{
}

ecs::World::~World()
{
	mSystems.clear();
}

ecs::Entity ecs::World::CreateEntity()
{
	Entity entity = mNextEntity++;
	mEntityToComponent[entity] = {};

	AddComponent(entity, Name());
	AddComponent(entity, Tag());
	AddComponent(entity, ChildParent());
	AddComponent(entity, Transform());

	return entity;
}

ecs::Entity ecs::World::CreateEntity(Entity aId)
{
	Entity entity = aId;
	auto it = mEntityToComponent.find(aId);
	if (it != mEntityToComponent.end())
	{
		std::cerr << "Entity already exists" << std::endl;
		return 0;
	}

	mEntityToComponent[entity] = {};

	AddComponent(entity, Name());
	AddComponent(entity, Tag());
	AddComponent(entity, ChildParent());
	AddComponent(entity, Transform());

	return entity;
}

ecs::Entity ecs::World::CreateEntity(const std::string& aName)
{
	Entity entity = mNextEntity++;
	mEntityToComponent[entity] = {};

	Name nameComp;
	nameComp.name = aName;
	AddComponent(entity, nameComp);
	AddComponent(entity, Tag());
	AddComponent(entity, ChildParent());
	AddComponent(entity, Transform());

	return entity;
}

ecs::Entity ecs::World::CreateEntity(Entity aId, const std::string& aName)
{
	Entity entity = aId;
	auto it = mEntityToComponent.find(aId);
	if (it != mEntityToComponent.end())
	{
		std::cerr << "Entity already exists" << std::endl;
		return 0;
	}

	mEntityToComponent[entity] = {};

	Name nameComp;
	nameComp.name = aName;
	AddComponent(entity, nameComp);
	AddComponent(entity, Tag());
	AddComponent(entity, ChildParent());
	AddComponent(entity, Transform());

	return entity;
}

void ecs::World::AddComponent(Entity anEntity, const std::vector<char>& someData, const reflection::TypeInfo& aType)
{
	if (someData.size() > aType.size)
		return;

	{
		ComponentMask mask = GetEntityComponentMask(anEntity);

		auto itToDelete = std::find(mMaskToEntity[mask].begin(), mMaskToEntity[mask].end(), anEntity);
		mMaskToEntity[mask].erase(itToDelete);
	}

	const std::type_index& componentType = *aType.type;
	auto it = mComponentAllocators.find(componentType);
	if (it == mComponentAllocators.end())
		mComponentAllocators[componentType] = ComponentAllocator(aType);

	void* compPtr = mComponentAllocators[componentType].Allocate();
	memcpy_s(compPtr, aType.size, someData.data(), someData.size());

	mEntityToComponent[anEntity][componentType] = compPtr;

	ComponentMask mask = GetEntityComponentMask(anEntity);
	mMaskToEntity[mask].push_back(anEntity);
}

std::unordered_map<ecs::ComponentType, void*> ecs::World::GetAllEntityComponents(Entity anEntity)
{
	return mEntityToComponent[anEntity];
}

void ecs::World::ProcessUpdateSystems()
{
	for (Pipeline pipeline = OnPreUpdate; pipeline < OnPostUpdate + 1; pipeline++)
	{
		for (auto& systemPtr : mSystems[pipeline])
		{
			for (auto& [mask, entityVector] : mMaskToEntity)
			{
				if ((mask & systemPtr->signature) == systemPtr->signature)
				{
					for (Entity entity : mMaskToEntity[systemPtr->signature])
					{
						auto& system = *systemPtr;
						auto func = std::any_cast<SystemFunction>(system.function);
						func(*this, entity);
					}
				}
			}
		}
	}
}

void ecs::World::ProcessInitSystems()
{
	for (Pipeline pipeline = OnLoad; pipeline < OnPostLoad + 1; pipeline++)
	{
		for (auto& systemPtr : mSystems[pipeline])
		{
			for (auto& [mask, entityVector] : mMaskToEntity)
			{
				if ((mask & systemPtr->signature) == systemPtr->signature)
				{
					for (Entity entity : entityVector)
					{
						auto& system = *systemPtr;
						auto func = std::any_cast<SystemFunction>(system.function);
						func(*this, entity);
					}
				}
			}
		}
	}
}

const std::unordered_map<ecs::ComponentMask, std::vector<ecs::Entity>> &ecs::World::GetAllEntitiesWithMask() const
{
	return mMaskToEntity;
}

std::vector<ecs::Entity> ecs::World::GetAllEntites()
{
	std::vector<ecs::Entity> output;
	output.reserve(mEntityToComponent.size());
	for (auto& entity : mEntityToComponent)
	{
		output.push_back(entity.first);
	}
	return output;
}

void ecs::World::AddSystem(const std::string& aSystemName)
{
	std::shared_ptr<System> system = Engine::GetECSSystemManager().GetSystemByName(aSystemName);
	mSystems[system->pipeline].push_back(system);
}

