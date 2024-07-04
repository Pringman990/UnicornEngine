#include "EnginePch.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>
#include <scene/Scene.h>
#include <fstream>

SceneSerializer::SceneSerializer()
{
}

SceneSerializer::~SceneSerializer()
{
}

bool SceneSerializer::SerializeYAML(const std::wstring& aFilePath, Scene& aScene)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "ID";
	out << YAML::Value << aScene.GetID();
	out << YAML::EndMap;

	std::ofstream ofs(aFilePath);
	if (ofs.is_open())
	{
		ofs << out.c_str();
	}
	ofs.close();
	return true;
}

bool SceneSerializer::SerializeBinary(const std::wstring& /*aFilePath*/, Scene& /*aScene*/)
{
	return false;
}

bool SceneSerializer::DeserializeYAML(const std::wstring& /*aFilePath*/, Scene& /*aScene*/)
{
	return false;
}

bool SceneSerializer::DeserializeBinary(const std::wstring& /*aFilePath*/, Scene& /*aScene*/)
{
	return false;
}

