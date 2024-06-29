#pragma once
#include <cstddef>

namespace reflection
{
	struct MemberInfo
	{
		std::string name;
		size_t offset = 0;
		size_t size = 0;
		std::type_index type;
	};

	struct TypeInfo
	{
		std::string name;
		size_t size = 0;
		std::vector<MemberInfo> members;
	};

	class Registry
	{
	public:
		static Registry& GetInstance()
		{
			static Registry instance;
			return instance;
		}

		void ShutDown();

		void RegisterType(std::type_index aType, TypeInfo aInfo);

		TypeInfo GetTypeInfo(std::type_index aType);

	private:
		Registry() {};
		~Registry() {};

	private:
		std::unordered_map<std::type_index, TypeInfo> mRegistry;
	};

#define REGISTER_COMPONENT(TYPE, MEMBERS) \
    struct Register##TYPE { \
        Register##TYPE() { \
            reflection::TypeInfo info; \
            info.name = #TYPE; \
            info.size = sizeof(TYPE); \
            MEMBERS \
            reflection::Registry::GetInstance().RegisterType(typeid(TYPE), info); \
        } \
    }; \
    Register##TYPE globalRegister##TYPE;

#define REGISTER_MEMBER(TYPE, MEMBER) \
    info.members.push_back({#MEMBER, offsetof(TYPE, MEMBER), sizeof(TYPE::MEMBER), typeid(decltype(TYPE::MEMBER))});

}