#pragma once

namespace ecs
{
	struct Mesh;
	struct Transform;

	namespace engineSystem
	{
		extern void MeshRenderSystem(ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Mesh& mesh);
	}
}