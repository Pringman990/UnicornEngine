#pragma once
#include <source/reflection/Reflection.h>
#include <source/model/ModelInstance.h>

namespace ecs
{
	struct Mesh
	{
		const wchar_t* meshPath = L"";
		ModelInstance modelInstance;

		REGISTER_COMPONENT(Mesh,
			REGISTER_MEMBER(Mesh, meshPath)
			REGISTER_MEMBER(Mesh, modelInstance)
		)
	};
}