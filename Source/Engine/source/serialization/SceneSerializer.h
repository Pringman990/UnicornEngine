#pragma once
#include "utility/yamlHelper.h"
#include "reflection/Reflection.h"

class Scene;

class SceneSerializer
{
public:

	bool SerializeYAML(const std::wstring& aFilePath, Scene& aScene);
	bool SerializeBinary(const std::wstring& aFilePath, Scene& aScene);

	bool DeserializeYAML(const std::wstring& aFilePath, Scene& aScene);
	bool DeserializeBinary(const std::wstring& aFilePath, Scene& aScene);

private:
	friend class Engine;
	SceneSerializer();
	~SceneSerializer();

	void SavePrimitiveTypes(YAML::Emitter& aEmitter, void*& aComponent, const reflection::MemberInfo& aMember);
	void SavePrimitiveVectorTypes(YAML::Emitter& aEmitter, void*& aComponent, const reflection::MemberInfo& aMember);
	bool IsVectorType(const std::type_index& aType);

	void LoadPrimitiveTypes(YAML::Node aMember, char*& aMemberPosition, const std::string& aMemberName);
private:

};