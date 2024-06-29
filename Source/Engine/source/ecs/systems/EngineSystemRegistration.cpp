#include "EnginePch.h"
#include "EngineSystemRegistration.h"

//Systems
#include "MeshRenderSystem.h"
#include "SpriteRenderSystem.h"

//Components
#include "../components/Transform.h"
#include "../components/Mesh.h"
#include "../components/Sprite.h"

extern ecs::World gECSWorld;

namespace ecs
{
	void ecs::RegisterSystems()
	{
		{
			auto meshSystem = [](ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Mesh& mesh) {
				engineSystem::MeshRenderSystem(world, entity, transform, mesh);
				};

			gECSWorld.BindSystem<ecs::Transform, ecs::Mesh>(meshSystem, OnEngineRender);
		}

		{
			auto spriteSystem = [](ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Sprite& sprite) {
				engineSystem::SpriteRenderSystem(world, entity, transform, sprite);
				};

			gECSWorld.BindSystem<ecs::Transform, ecs::Sprite>(spriteSystem, OnEngineRender);
		}
	}
}
