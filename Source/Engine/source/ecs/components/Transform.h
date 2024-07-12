#pragma once
#include <source/reflection/Reflection.h>
#include <source/serialization/SceneSerializer.h>

namespace ecs
{
	struct Transform
	{
		Vector3 position = { 0,0,0 };
		Vector3 rotation = { 0,0,0 };
		Vector3 scale = { 1,1,1 };

	};
	REGISTER_COMPONENT(Transform,
		REGISTER_MEMBER(Transform, position)
		REGISTER_MEMBER(Transform, rotation)
		REGISTER_MEMBER(Transform, scale)
	)
}

