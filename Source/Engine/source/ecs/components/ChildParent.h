#pragma once
#include "reflection/Reflection.h"

namespace ecs
{
	struct ChildParent
	{
		Entity parent;
		std::vector<Entity> children;
		
		REGISTER_COMPONENT(ChildParent,
			REGISTER_MEMBER(ChildParent, parent)
			REGISTER_MEMBER(ChildParent, children)
		)
	};
}