#pragma once
#include <reflection/Reflection.h>
#include <model/ModelInstance.h>
#include <model/ModelFactory.h>
#include <GraphicsEngine.h>

namespace ecs
{
	struct Mesh
	{
		Mesh() {};

		Mesh(const wchar_t* aWPath)
		{
			modelInstance =	Engine::GetGraphicsEngine().GetModelFactory().GetModelInstance(aWPath);
			meshPath = aWPath;
		}

		const wchar_t* meshPath = L"";
		ModelInstance modelInstance;

		REGISTER_COMPONENT(Mesh,
			REGISTER_MEMBER(Mesh, meshPath)
			REGISTER_MEMBER(Mesh, modelInstance)
		)
	};
}