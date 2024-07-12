#pragma once
#include "reflection/Reflection.h"

namespace ecs
{
	struct Name
	{
		std::string name = "Unnamed";

	};
		REGISTER_COMPONENT(Name,
			REGISTER_MEMBER(Name, name)
		)
}