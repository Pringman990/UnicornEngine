#pragma once
namespace ecs
{
	struct Sprite;
	struct Transform;

	namespace engineSystem
	{
		extern void SpriteRenderSystem(ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Sprite& sprite);
	}
}