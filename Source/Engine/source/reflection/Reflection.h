#pragma once
#include <cstddef>
#include <yaml-cpp/yaml.h>

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

		//Serializing
		std::function<void(YAML::Emitter&, void*&)> saveFunction;
		std::function<void(YAML::Node, void*&)> loadFunction;
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
		TypeInfo GetTypeInfo(const std::string& aName);

	private:
		Registry();
		~Registry();

	private:
		std::unordered_map<std::type_index, TypeInfo> mRegistry;
		std::unordered_map<std::string, TypeInfo> mNameToType;
	};

#define REGISTER_COMPONENT(TYPE, MEMBERS) \
    namespace { \
        void Register##TYPE() { \
            reflection::TypeInfo info; \
            info.name = #TYPE; \
            info.size = sizeof(TYPE); \
            info.type = std::make_shared<std::type_index>(typeid(TYPE)); \
            MEMBERS \
            info.createInstance = []() -> std::vector<char> { \
                TYPE instance; \
                std::vector<char> vector; \
                vector.resize(sizeof(TYPE)); \
                std::memcpy(vector.data(), &instance, sizeof(TYPE)); \
                return vector; \
            }; \
            reflection::Registry::GetInstance().RegisterType(typeid(TYPE), info); \
        } \
        static bool dummy##TYPE = (Register##TYPE(), true); \
    }

#define REGISTER_MEMBER(TYPE, MEMBER) \
    info.members.push_back({#MEMBER, offsetof(TYPE, MEMBER), sizeof(TYPE::MEMBER), typeid(decltype(TYPE::MEMBER))});

#define REGISTER_SAVE_FUNCTION(TYPE, FUNCTION) \
	 namespace { \
		void Save##TYPE() { \
		reflection::TypeInfo info = reflection::Registry::GetInstance().GetTypeInfo(typeid(TYPE)); \
		info.saveFunction = [](YAML::Emitter& out, void*& comp)  \
			{\
				FUNCTION(out, comp);\
			};\
		reflection::Registry::GetInstance().RegisterType(typeid(TYPE), info); \
		} \
		static bool savedummy##TYPE = (Save##TYPE(), true); \
	} 
	

#define REGISTER_LOAD_FUNCTION(TYPE, FUNCTION) \
	namespace { \
	 void Load##TYPE() { \
		reflection::TypeInfo info = reflection::Registry::GetInstance().GetTypeInfo(typeid(TYPE)); \
		info.loadFunction = [](YAML::Node node, void*& comp)  \
			{\
				FUNCTION(node, comp);\
			};\
		reflection::Registry::GetInstance().RegisterType(typeid(TYPE), info); \
		} \
		static bool loaddummy##TYPE = (Load##TYPE(), true); \
	} 	
}