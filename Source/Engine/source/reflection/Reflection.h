#pragma once
#include <cstddef>

class Engine;

namespace reflection
{
	struct MemberInfo
	{
		std::string name = "_INVALID";
		size_t offset = 0;
		size_t size = 0;
		std::type_index type;
	};

	struct TypeInfo
	{
		std::string name = "_INVALID";
		size_t size = 0;
		std::vector<MemberInfo> members;
		std::shared_ptr<std::type_index> type;
		std::function<std::vector<char>(void)> createInstance;
	};

	class Registry
	{
		friend class Engine;
	public:
		void ShutDown();

		void RegisterType(std::type_index aType, TypeInfo aInfo);

		TypeInfo GetTypeInfo(std::type_index aType);
		TypeInfo GetTypeInfo(const std::string& aName);

	private:
		Registry();
		~Registry();

	private:
		std::unordered_map<std::type_index, TypeInfo> mRegistry;
		std::unordered_map<std::string, TypeInfo> mNameToType;
	};

#define REGISTER_COMPONENT(TYPE, MEMBERS) \
    struct Register##TYPE { \
        Register##TYPE() { \
            reflection::TypeInfo info; \
            info.name = #TYPE; \
            info.size = sizeof(TYPE); \
            info.type = std::make_shared<std::type_index>(typeid(TYPE)); \
            MEMBERS \
			info.createInstance = []() -> std::vector<char> \
			{ \
				TYPE instance;\
				std::vector<char> vector;\
				vector.resize(sizeof(TYPE));\
				memcpy_s(vector.data(), sizeof(TYPE), &instance, sizeof(TYPE));\
				return vector; \
			}; \
            Engine::GetReflectionRegistry().RegisterType(typeid(TYPE), info); \
        } \
    }; \
    Register##TYPE globalRegister##TYPE;

#define REGISTER_MEMBER(TYPE, MEMBER) \
    info.members.push_back({#MEMBER, offsetof(TYPE, MEMBER), sizeof(TYPE::MEMBER), typeid(decltype(TYPE::MEMBER))});
}