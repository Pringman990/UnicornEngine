#include "EnginePch.h"
#include "SceneSerializer.h"


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

	ecs::World& world = aScene.GetWorld();
	std::vector<ecs::Entity> entities = world.GetAllEntites();

	for (auto& entity : entities)
	{
		out << YAML::BeginDoc;
		out << YAML::BeginMap;
		out << YAML::Key << "Entity id" << YAML::Value << entity;
		out << YAML::Key << "Components";

		out << YAML::BeginSeq;

		auto entityToComponent = world.GetAllEntityComponents(entity);
		auto& reflect = reflection::Registry::GetInstance();
		for (auto& [componentType, componentPtr] : entityToComponent)
		{
			reflection::TypeInfo typeInfo = reflect.GetTypeInfo(componentType);
			if (typeInfo.saveFunction)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "name" << YAML::Value << typeInfo.name;
				out << YAML::Key << "members";
				out << YAML::BeginSeq;
				out << YAML::BeginMap;
				typeInfo.saveFunction(out, componentPtr);
				out << YAML::EndMap;
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			else
			{

				out << YAML::BeginMap;
				out << YAML::Key << "name" << YAML::Value << typeInfo.name;
				out << YAML::Key << "size" << YAML::Value << typeInfo.size;
				out << YAML::Key << "members";
				out << YAML::BeginSeq;
				for (auto& member : typeInfo.members)
				{
					out << YAML::BeginMap;
					SavePrimitiveTypes(out, componentPtr, member);
					out << YAML::Key << "type" << YAML::Value << member.type.name();
					out << YAML::Key << "size" << YAML::Value << member.size;
					out << YAML::Key << "offset" << YAML::Value << member.offset;
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
		out << YAML::EndDoc;
	}

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

bool SceneSerializer::DeserializeYAML(const std::wstring& aFilePath, Scene& aScene)
{
	std::vector<YAML::Node> file = YAML::LoadAllFromFile(WStringToString(aFilePath));
	
	for (size_t i = 0; i < file.size(); i++)
	{
		YAML::Node entityDoc = file[i];

		if (!entityDoc["Entity id"])
			continue;

		ecs::Entity entity = aScene.GetWorld().CreateEntity(entityDoc["Entity id"].as<ecs::Entity>());

		if (entity == 0)
			continue;
		
		if (!entityDoc["Components"].IsSequence())
			continue;

		for (size_t j = 0; j < entityDoc["Components"].size(); j++)
		{
			YAML::Node componentDoc = entityDoc["Components"][j];

			if (componentDoc["name"].as<std::string>() == "_INVALID")
				continue;

			reflection::TypeInfo type = reflection::Registry::GetInstance().GetTypeInfo(componentDoc["name"].as<std::string>());
			std::vector<char> componentData = type.createInstance();

			void* ptr = componentData.data();
			if (type.loadFunction)
			{
				type.loadFunction(componentDoc["members"], ptr);
				aScene.GetWorld().AddComponent(entity, componentData, type);
				continue;
			}
			
			if (componentDoc["members"].size() != type.members.size())
				continue;

			char* firstMember = componentData.data();
			for (size_t k = 0; k < componentDoc["members"].size(); k++)
			{
				YAML::Node memberNode = componentDoc["members"][k];
				char* memberPosition = firstMember + memberNode["offset"].as<uint32_t>();
				
				LoadPrimitiveTypes(memberNode, memberPosition, type.members[k].name);
			}

			aScene.GetWorld().AddComponent(entity, componentData, type);
		}
	}

	return false;
}

bool SceneSerializer::DeserializeBinary(const std::wstring& /*aFilePath*/, Scene& /*aScene*/)
{
	return false;
}

void SceneSerializer::SavePrimitiveTypes(YAML::Emitter& aEmitter, void*& aComponent, const reflection::MemberInfo& aMember)
{
	char* firstElement = static_cast<char*>(aComponent);
	char* wantedMemberPtr = firstElement + aMember.offset;
	
	if (IsVectorType(aMember.type))
	{
		SavePrimitiveVectorTypes(aEmitter, aComponent, aMember);
		return;
	}

	aEmitter << YAML::Key << aMember.name;
	
	if (aMember.type == typeid(int))
	{
		int dest;
		memcpy_s(&dest, sizeof(int), wantedMemberPtr, sizeof(int));

		aEmitter << YAML::Value << dest;
	}
	if (aMember.type == typeid(ecs::Entity))
	{
		ecs::Entity dest;
		memcpy_s(&dest, sizeof(ecs::Entity), wantedMemberPtr, sizeof(ecs::Entity));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(float))
	{
		float dest;
		memcpy_s(&dest, sizeof(float), wantedMemberPtr, sizeof(float));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(double))
	{
		double dest;
		memcpy_s(&dest, sizeof(double), wantedMemberPtr, sizeof(double));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(std::string))
	{
		std::string stringPtr = *reinterpret_cast<std::string*>(wantedMemberPtr);
		aEmitter << YAML::Value << stringPtr;
	}
	else if (aMember.type == typeid(std::wstring))
	{
		std::wstring stringPtr = *reinterpret_cast<std::wstring*>(wantedMemberPtr);

		aEmitter << YAML::Value << stringPtr;
	}
	else if (aMember.type == typeid(const char*))
	{
		const char* stringPtr = reinterpret_cast<const char*>(wantedMemberPtr);

		aEmitter << YAML::Value << stringPtr;
	}
	else if (aMember.type == typeid(const wchar_t*))
	{
		const wchar_t* stringPtr = reinterpret_cast<const wchar_t*>(wantedMemberPtr);

		aEmitter << YAML::Value << stringPtr;
	}
	else if (aMember.type == typeid(Vector2))
	{
		Vector2 dest;
		memcpy_s(&dest, sizeof(Vector2), wantedMemberPtr, sizeof(Vector2));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(Vector3))
	{
		Vector3 dest;
		memcpy_s(&dest, sizeof(Vector3), wantedMemberPtr, sizeof(Vector3));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(Vector4))
	{
		Vector4 dest;
		memcpy_s(&dest, sizeof(Vector4), wantedMemberPtr, sizeof(Vector4));

		aEmitter << YAML::Value << dest;
	}
	else if (aMember.type == typeid(Matrix))
	{
		Matrix dest;
		memcpy_s(&dest, sizeof(Matrix), wantedMemberPtr, sizeof(Matrix));

		aEmitter << YAML::Value << dest;
	}
	else
	{
		aEmitter << YAML::Value << "_UNKOWN_TYPE";
	}
}

void SceneSerializer::SavePrimitiveVectorTypes(YAML::Emitter& aEmitter, void*& aComponent, const reflection::MemberInfo& aMember)
{
	char* firstElement = static_cast<char*>(aComponent);
	char* wantedMemberPtr = firstElement + aMember.offset;

	aEmitter << YAML::Key << aMember.name;

	if (aMember.type == typeid(std::vector<int>))
	{
		std::vector<int>* vectorPtr = reinterpret_cast<std::vector<int>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << *vectorPtr;
	}
	if (aMember.type == typeid(std::vector<ecs::Entity>))
	{
		std::vector<ecs::Entity>* vectorPtr = reinterpret_cast<std::vector<ecs::Entity>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<float>))
	{
		std::vector<float>* vectorPtr = reinterpret_cast<std::vector<float>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<double>))
	{
		std::vector<double>* vectorPtr = reinterpret_cast<std::vector<double>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<std::string>))
	{
		std::vector<std::string>* vectorPtr = reinterpret_cast<std::vector<std::string>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<std::wstring>))
	{
		std::vector<std::wstring>* vectorPtr = reinterpret_cast<std::vector<std::wstring>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<const char*>))
	{
		std::vector<const char*>* vectorPtr = reinterpret_cast<std::vector<const char*>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector<const wchar_t*>))
	{
		std::vector<const wchar_t*>* vectorPtr = reinterpret_cast<std::vector<const wchar_t*>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector < Vector2>))
	{
		std::vector<Vector2>* vectorPtr = reinterpret_cast<std::vector<Vector2>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector < Vector3>))
	{
		std::vector<Vector3>* vectorPtr = reinterpret_cast<std::vector<Vector3>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector < Vector4>))
	{
		std::vector<Vector4>* vectorPtr = reinterpret_cast<std::vector<Vector4>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else if (aMember.type == typeid(std::vector < Matrix>))
	{
		std::vector<Matrix>* vectorPtr = reinterpret_cast<std::vector<Matrix>*>(wantedMemberPtr);

		aEmitter << YAML::Value << YAML::Flow << vectorPtr;
	}
	else
	{
		aEmitter << YAML::Value << "_UNKOWN_TYPE";
	}
}

bool SceneSerializer::IsVectorType(const std::type_index& aType)
{
	if (aType == typeid(std::vector<int>)) return true;
	if (aType == typeid(std::vector<ecs::Entity>)) return true;
	if (aType == typeid(std::vector<float>)) return true;
	if (aType == typeid(std::vector<double>)) return true;
	if (aType == typeid(std::vector<std::string>)) return true;
	if (aType == typeid(std::vector<std::wstring>)) return true;
	if (aType == typeid(std::vector<const char*>)) return true;
	if (aType == typeid(std::vector<const wchar_t*>)) return true;
	if (aType == typeid(std::vector<Vector2>)) return true;
	if (aType == typeid(std::vector<Vector3>)) return true;
	if (aType == typeid(std::vector<Vector4>)) return true;
	if (aType == typeid(std::vector<Matrix>)) return true;

	return false;
}

void SceneSerializer::LoadPrimitiveTypes(YAML::Node aMemberNode, char*& aMemberPosition, const std::string& aMemberName)
{
	std::string typeName = aMemberNode["type"].as<std::string>();

	if (typeName == typeid(int).name())
	{
		int value = aMemberNode[aMemberName].as<int>();
		memcpy_s(aMemberPosition, sizeof(int), &value, sizeof(int));
	}
	else if (typeName == typeid(ecs::Entity).name())
	{
		ecs::Entity value = aMemberNode[aMemberName].as<ecs::Entity>();
		memcpy_s(aMemberPosition, sizeof(ecs::Entity), &value, sizeof(ecs::Entity));
	}
	else if (typeName == typeid(float).name())
	{
		float value = aMemberNode[aMemberName].as<float>();
		memcpy_s(aMemberPosition, sizeof(float), &value, sizeof(float));
	}
	else if (typeName == typeid(double).name())
	{
		double value = aMemberNode[aMemberName].as<double>();
		memcpy_s(aMemberPosition, sizeof(double), &value, sizeof(double));
	}
	else if (typeName == typeid(std::string).name())
	{
		std::string* str = reinterpret_cast<std::string*>(aMemberPosition);
		std::string newStr = aMemberNode[aMemberName].as<std::string>();
		*str = newStr;
	}
	else if (typeName == typeid(std::wstring).name())
	{
		std::wstring* str = reinterpret_cast<std::wstring*>(aMemberPosition);
		std::wstring newStr = aMemberNode[aMemberName].as<std::wstring>();
		*str = newStr;
	}
	else if (typeName == typeid(const char*).name())
	{
		const char** cstr = reinterpret_cast<const char**>(aMemberPosition);

		std::string str = aMemberNode[aMemberName].as<std::string>();

		char* buffer = new char[str.size() + 1];
		strcpy_s(buffer, str.size() + 1, str.c_str());
		*cstr = buffer;
	}
	else if (typeName == typeid(const wchar_t*).name())
	{
		const wchar_t** str = reinterpret_cast<const wchar_t**>(aMemberPosition);
		std::wstring wstr = aMemberNode[aMemberName].as<std::wstring>(); 

		wchar_t* buffer = new wchar_t[wstr.size() + 1];
		wcscpy_s(buffer, wstr.size() + 1, wstr.c_str());
		*str = buffer;
	}
	else if (typeName == typeid(Vector2).name())
	{
		Vector2 value = aMemberNode[aMemberName].as<Vector2>();
		memcpy_s(aMemberPosition, sizeof(Vector2), &value, sizeof(Vector2));
	}
	else if (typeName == typeid(Vector3).name())
	{
		Vector3 value = aMemberNode[aMemberName].as<Vector3>();
		memcpy_s(aMemberPosition, sizeof(Vector3), &value, sizeof(Vector3));
	}
	else if (typeName == typeid(Vector4).name())
	{
		Vector4 value = aMemberNode[aMemberName].as<Vector4>();
		memcpy_s(aMemberPosition, sizeof(Vector4), &value, sizeof(Vector4));
	}
	else if (typeName == typeid(Matrix).name())
	{
		Matrix value = aMemberNode[aMemberName].as<Matrix>();
		memcpy_s(aMemberPosition, sizeof(Matrix), &value, sizeof(Matrix));
	}
	else
	{
		std::cout << "Loaded type was not primitive" << std::endl;
	}
}
