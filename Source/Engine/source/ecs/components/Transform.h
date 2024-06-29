#pragma once
#include <source/reflection/Reflection.h>

namespace ecs
{
	struct Transform
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale = {1,1,1};

		REGISTER_COMPONENT(Transform,
			REGISTER_MEMBER(Transform, position)
			REGISTER_MEMBER(Transform, rotation)
			REGISTER_MEMBER(Transform, scale)
		)
	};
}