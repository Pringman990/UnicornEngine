#include "EnginePch.h"

#include "scene/SceneManager.h"

void ecs::SystemManager::RunUpdateEngineSystems()
{
	ecs::World& currentWorld = Engine::GetSceneManager().GetCurrentScene().GetWorld();
	auto maskToEntites = currentWorld.GetAllEntitiesWithMask();
	for (Pipeline pipeline = OnLoad; pipeline < OnPostLoad + 1; pipeline++)
	{
		for (auto& system : mEngineSystems[pipeline])
		{
			for (auto& [mask, entityVector] : maskToEntites)
			{
				if ((mask & system->signature) == system->signature)
				{
					for (Entity entity : entityVector)
					{
						system->function(currentWorld, entity);
					}
				}
			}
		}
	}
}

void ecs::SystemManager::RunRenderEngineSystems()
{
	ecs::World& currentWorld = Engine::GetSceneManager().GetCurrentScene().GetWorld();
	auto maskToEntites = currentWorld.GetAllEntitiesWithMask();

	for (auto& system : mEngineSystems[OnRender])
	{
		for (auto& [mask, entityVector] : maskToEntites)
		{
			if ((mask & system->signature) == system->signature)
			{
				for (Entity entity : entityVector)
				{
					system->function(currentWorld, entity);
				}
			}
		}
	}
}

std::shared_ptr<ecs::System> ecs::SystemManager::GetSystemByName(const std::string& aName)
{
	return std::make_shared<System>(mSystems[aName]);
}
