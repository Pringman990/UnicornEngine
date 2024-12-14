#pragma once

struct EMesh
{
	EMesh() {};

	EMesh(const wchar_t* aWPath)
	{
		//modelInstance = Engine::GetRenderer().GetModelFactory().GetModelInstance(aWPath);
		meshPath = aWPath;
	}

	const wchar_t* meshPath = L"";
	//std::shared_ptr<ModelInstance> modelInstance;


	//inline static void Save(YAML::Emitter& out, void*& aComponent) {
	//	ecs::EMesh* mesh = static_cast<ecs::EMesh*>(aComponent);

	//	out << YAML::Key << "meshPath" << YAML::Value << std::wstring(mesh->meshPath);
	//}

	//inline static void Load(YAML::Node node, void*& aComponent) {
	//	ecs::EMesh* mesh = static_cast<ecs::EMesh*>(aComponent);

	//	//we do node[0] because meshPath is the the first member in the sequence
	//	std::wstring wstr = node[0]["meshPath"].as<std::wstring>();

	//	wchar_t* buffer = new wchar_t[wstr.size() + 1];
	//	wcscpy_s(buffer, wstr.size() + 1, wstr.c_str());

	//	mesh->meshPath = buffer;

	//	mesh->modelInstance = Engine::GetGraphicsEngine().GetModelFactory().GetModelInstance(mesh->meshPath);
	//}

};

REGISTER_COMPONENT(EMesh,
	REGISTER_MEMBER(EMesh, EMesh::meshPath)
)
