#pragma once
#include <Singleton.h>

#include <vector>
#include <memory>
#include <typeindex>
#include <string>
#include <functional>

struct ReflectionMemberInfo
{
	std::string name = "_INVALID";
	size_t offset = 0;
	size_t size = 0;
	std::type_index type;
};

struct ReflectionTypeInfo
{
	std::string name = "_INVALID";
	size_t size = 0;
	std::vector<ReflectionMemberInfo> members;
	std::shared_ptr<std::type_index> type;
	std::function<std::vector<char>(void)> createInstance;

	//Serializing
	//std::function<void(YAML::Emitter&, void*&)> saveFunction;
	//std::function<void(YAML::Node, void*&)> loadFunction;
};

class ReflectionRegistry : public Singleton<ReflectionRegistry>
{
public:
	void RegisterType(std::type_index aType, ReflectionTypeInfo aInfo);

	ReflectionTypeInfo GetTypeInfo(std::type_index aType);
	ReflectionTypeInfo GetTypeInfo(const std::string& aName);

private:
	friend class Singleton<ReflectionRegistry>;
	ReflectionRegistry();
	~ReflectionRegistry();
private:
	std::unordered_map<std::type_index, ReflectionTypeInfo> mRegistry;
	std::unordered_map<std::string, ReflectionTypeInfo> mNameToType;
};

#define REGISTER_COMPONENT(TYPE, MEMBERS) \
    namespace { \
        void Register##TYPE() { \
            ReflectionTypeInfo info; \
            info.name = #TYPE; \
            info.size = sizeof(TYPE); \
            info.type = std::make_shared<std::type_index>(typeid(TYPE)); \
            MEMBERS \
            info.createInstance = []() -> std::vector<char> { \
                TYPE instance = {}; \
                std::vector<char> vector; \
                vector.resize(sizeof(TYPE)); \
                std::memcpy(vector.data(), &instance, sizeof(TYPE)); \
                return vector; \
            }; \
            ReflectionRegistry::GetInstance()->RegisterType(typeid(TYPE), info); \
        } \
        static bool dummy##TYPE = (Register##TYPE(), true); \
    }

#define REGISTER_MEMBER(TYPE, MEMBER) \
    info.members.push_back({#MEMBER, offsetof(TYPE, MEMBER), sizeof(TYPE::MEMBER), typeid(decltype(TYPE::MEMBER))});

//#define ECOMPONENT(...)
//#define EMEMBER(...)
//
////EMember properties
//enum {
//	HideInInspector,
//	Const,
//	Category
//};
//
//ECOMPONENT()
//struct Transform
//{
//	EMEMBER(HideInInspector, Const)
//	int a;
//};
//
//REGISTER_COMPONENT(Transform, 
//	REGISTER_MEMBER(Transform, Transform::a)
//)