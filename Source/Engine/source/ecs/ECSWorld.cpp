#include "EnginePch.h"
#include "ECSWorld.h"

ecs::World::World()
{
}

ecs::World::~World()
{
	mSystems.clear();
}

ecs::Entity ecs::World::CreateEntity()
{
	Entity entity = mNextEntity++;
	mEntityToIndex[entity] = {};
	mMaskToEntity[0].push_back(entity);
	return entity;
}

std::unordered_map<ecs::ComponentType, void*> ecs::World::GetAllEntityComponents(Entity anEntity)
{
	std::unordered_map<ComponentType, void*> returnMap;

	for (auto& [type, index] : mEntityToIndex[anEntity])
	{
		returnMap.insert({ type, mComponentArrays[type] });
	}

	return returnMap;
}

void ecs::World::ProcessEngineRenderSystems()
{
	for (auto& [signature, system] : mSystems[OnEngineRender])
	{
		for (auto& [mask, entityVector] : mMaskToEntity)
		{
			if ((mask & signature) == signature)
			{
				for (Entity entity : entityVector)
				{
					system(*this, entity);
				}
			}
		}
	}

}

void ecs::World::ProcessUpdateSystems()
{
	for (Pipeline pipeline = OnPreUpdate; pipeline < OnPostUpdate + 1; pipeline++)
	{
		for (auto& [signature, system] : mSystems[pipeline])
		{
			for (auto& [mask, entityVector] : mMaskToEntity)
			{
				if ((mask & signature) == signature)
				{
					for (Entity entity : entityVector)
					{
						system(*this, entity);
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
		for (auto& [signature, system] : mSystems[pipeline])
		{
			for (auto& [mask, entityVector] : mMaskToEntity)
			{
				if ((mask & signature) == signature)
				{
					for (Entity entity : entityVector)
					{
						system(*this, entity);
					}
				}
			}
		}
	}
}

