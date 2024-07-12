#pragma once
#include <reflection/Reflection.h>
#include <model/ModelInstance.h>
#include <model/ModelFactory.h>
#include <GraphicsEngine.h>
#include <utility/yamlHelper.h>

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


		inline static void Save(YAML::Emitter& out, void*& aComponent) {
			ecs::Mesh* mesh = static_cast<ecs::Mesh*>(aComponent);

			out << YAML::Key << "meshPath" << YAML::Value << std::wstring(mesh->meshPath);
		}

		inline static void Load(YAML::Node node, void*& aComponent) {
			ecs::Mesh* mesh = static_cast<ecs::Mesh*>(aComponent);

			//we do node[0] because meshPath is the the first member in the sequence
			std::wstring wstr = node[0]["meshPath"].as<std::wstring>();

			wchar_t* buffer = new wchar_t[wstr.size() + 1];
			wcscpy_s(buffer, wstr.size() + 1, wstr.c_str());

			mesh->meshPath = buffer;

			mesh->modelInstance = Engine::GetGraphicsEngine().GetModelFactory().GetModelInstance(mesh->meshPath);
		}

	};
		REGISTER_COMPONENT(Mesh,
			REGISTER_MEMBER(Mesh, meshPath)
			REGISTER_MEMBER(Mesh, modelInstance)
		)

		REGISTER_SAVE_FUNCTION(Mesh, Mesh::Save)
		REGISTER_LOAD_FUNCTION(Mesh, Mesh::Load)
}