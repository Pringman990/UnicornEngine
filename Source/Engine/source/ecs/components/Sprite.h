#pragma once
#include <source/reflection/Reflection.h>
#include <source/sprite/SpriteInstance.h>

namespace ecs
{
	struct Sprite
	{
		SpriteInstance sprite;

		REGISTER_COMPONENT(Sprite, 
			REGISTER_MEMBER(Sprite, sprite)
		)
	};
}