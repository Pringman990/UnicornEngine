#pragma once
#include <Singleton.h>

#include <vector>
#include <memory>
#include <typeindex>
#include <string>
#include <functional>


#define _DEBUG_PRINT_REFLECTION_REGISTRY 1

#if _DEBUG_PRINT_REFLECTION_REGISTRY
#define REFLECTION_LOG_TYPE_INFO(INFO) \
        _LOG_CORE_INFO("======================="); \
        _LOG_CORE_INFO("Name: {}", INFO.name); \
        _LOG_CORE_INFO("Size: {}", INFO.size); \
        _LOG_CORE_INFO("Alignment: {}", INFO.alignment); \
        _LOG_CORE_INFO("IsTrivial: {}", INFO.isTrivial);\
        _LOG_CORE_INFO("----------------"); 

#define REFLECTION_LOG_MEMBER_INFO(TYPE, MEMBER) \
        _LOG_CORE_INFO("Member Name: {}", #MEMBER); \
        _LOG_CORE_INFO("Offset: {}", offsetof(TYPE, MEMBER)); \
        _LOG_CORE_INFO("Size: {}", sizeof(TYPE::MEMBER));\
        _LOG_CORE_INFO("----------------"); 
#else
#define REFLECTION_LOG_TYPE_INFO(INFO)
#define REFLECTION_LOG_MEMBER_INFO(TYPE, MEMBER)
#endif

struct ReflectionMemberInfo
{
	std::string name = "_INVALID";
	size_t offset = 0;
	size_t size = 0;
	std::type_index typeIndex;
};

struct ReflectionTypeInfo
{
	std::string name = "_INVALID";
	size_t size = 0;
	size_t alignment = 0;
	bool isTrivial = false;

	std::vector<ReflectionMemberInfo> members;
	std::shared_ptr<std::type_index> typeIndex;

	std::function<void(void*, void*)> constructor;
	std::function<void(void*)> destructor;

	std::function<void(void* dst, void* src)> move;
	std::function<void(void* dst, void* src)> copy;
};

class ReflectionRegistry : public Singleton<ReflectionRegistry>
{
public:
	using DeffRegistartionFn = void(*)();

	void RegisterType(ReflectionTypeInfo aInfo);

	ReflectionTypeInfo GetTypeInfo(std::type_index aType);
	ReflectionTypeInfo GetTypeInfo(const std::string& aName);

	static void EnqueueDefferedRegistration(DeffRegistartionFn aRegFunction);
	static void ProcessDefferedRegistration();
private:
	friend class Singleton<ReflectionRegistry>;
	ReflectionRegistry();
	~ReflectionRegistry();

	static std::vector<DeffRegistartionFn>& GetDefferedQueue();
private:
	std::unordered_map<std::type_index, ReflectionTypeInfo> mRegistry;
	std::unordered_map<std::string, ReflectionTypeInfo> mNameToType;

};

#define REFLECTION_CREATE_TYPE_INFO(TYPE, MEMBERS) \
	ReflectionTypeInfo info; \
	info.name = #TYPE; \
	info.size = sizeof(TYPE); \
	info.alignment = alignof(TYPE); \
	info.typeIndex = std::make_shared<std::type_index>(typeid(TYPE)); \
	info.isTrivial = std::is_trivially_copyable_v<TYPE>; \
	info.constructor = [](void* mem, void* data) { new (mem) TYPE(std::move(*reinterpret_cast<TYPE*>(data))); }; \
	info.destructor = [](void* mem) { reinterpret_cast<TYPE*>(mem)->~TYPE(); }; \
	info.move = [](void* dst, void* src) { \
		if constexpr (std::is_trivially_copyable_v<TYPE>) { \
			std::memcpy(dst, src, sizeof(TYPE)); \
		} \
		else { \
			new (dst) TYPE(std::move(*reinterpret_cast<TYPE*>(src))); \
		} \
	}; \
	info.copy = [](void* dst, void* src) { new (dst) TYPE(*reinterpret_cast<TYPE*>(src)); }; \
	REFLECTION_LOG_TYPE_INFO(info) \
	MEMBERS \
	ReflectionRegistry::GetInstance()->RegisterType(info); 


#define REFLECTION_CREATE_MEMBER_INFO(TYPE, MEMBER) \
    info.members.push_back({#MEMBER, offsetof(TYPE, MEMBER), sizeof(TYPE::MEMBER), typeid(decltype(TYPE::MEMBER))});\
	REFLECTION_LOG_MEMBER_INFO(TYPE, MEMBER)