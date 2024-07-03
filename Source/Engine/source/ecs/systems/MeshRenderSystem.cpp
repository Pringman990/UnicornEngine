#include "EnginePch.h"
#include "MeshRenderSystem.h"

#include "../components/Transform.h"
#include "../components/Mesh.h"

#include <source/model/Model.h>

void ecs::engineSystem::MeshRenderSystem(ecs::World& /*world*/, ecs::Entity /*entity*/, ecs::Transform& transform, ecs::Mesh& mesh)
{
	if (!mesh.modelInstance.IsValid())
		return;

	mesh.modelInstance.GetModel()->GetTransform().SetPosition(transform.position);
	mesh.modelInstance.GetModel()->GetTransform().SetRotation(transform.rotation);
	mesh.modelInstance.GetModel()->GetTransform().SetScale(transform.scale);
	mesh.modelInstance.Render();
}
