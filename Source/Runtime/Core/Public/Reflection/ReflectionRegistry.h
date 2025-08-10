#pragma once
#include "StandardTypes/StandardTypes.h"
#include "EngineSubsystem.h"
#include "TypeIdHash.h"

#include <span>

#define _DEBUG_PRINT_REFLECTION_REGISTRY 1

#if _DEBUG_PRINT_REFLECTION_REGISTRY
#define REFLECTION_LOG_TYPE_INFO(INFO) \
        _LOG_CORE_INFO("======================="); \
        _LOG_CORE_INFO("Name: {}", INFO.name); \
        _LOG_CORE_INFO("Type: {}", INFO.type); \
        _LOG_CORE_INFO("Size: {}", INFO.size); \
		for(auto member : INFO.members) \
		{ \
			_LOG_CORE_INFO("----------------"); \
			_LOG_CORE_INFO("Member Name: {}", member.name); \
			_LOG_CORE_INFO("Type: {}", member.type); \
			_LOG_CORE_INFO("Offset: {}", member.offset); \
			_LOG_CORE_INFO("Size: {}", member.size); \
		} \
        _LOG_CORE_INFO("======================="); 
#else
#define REFLECTION_LOG_TYPE_INFO(INFO)
#endif

using ConstructFunc = void(*)(void* Dst);
using CopyFunc = void(*)(void* Dst, const void* Src);
using MoveFunc = void(*)(void* Dst, void* Src);
using DestroyFunc = void(*)(void* Obj);

class ReflectionRegistry : public EngineSubsystem<ReflectionRegistry>
{
public:
	struct MemberInfo
	{
		std::string_view name;
		TypeIdHash type;
		size_t offset;
		size_t size;

		static constexpr MemberInfo MakeInfo(std::string_view Name, TypeIdHash Type, size_t Offset, size_t Size)
		{
			return {
				Name,
				Type,
				Offset,
				Size
			};
		}
	};

	struct TypeInfo
	{
		std::string_view name{};
		TypeIdHash type{};
		size_t size{};
		std::span<const MemberInfo> members{};

		ConstructFunc constructFunc = nullptr;
		CopyFunc copyFunc = nullptr;
		MoveFunc moveFunc = nullptr;
		DestroyFunc destroyFunc = nullptr;

		constexpr TypeInfo() noexcept = default;

		constexpr TypeInfo(
			std::string_view n, 
			TypeIdHash t, 
			size_t s,
			std::span<const MemberInfo> m,
			ConstructFunc construct,
			CopyFunc copy,
			MoveFunc move,
			DestroyFunc destroy
		)
			: 
			name(n), 
			type(t), 
			size(s), 
			members(m),
			constructFunc(construct),
			copyFunc(copy),
			moveFunc(move),
			destroyFunc(destroy) {}

		template<typename T, typename R>
		static constexpr TypeInfo MakeInfo(std::string_view Name)
		{
			return {
				Name,
				GetTypeId<T>(),
				sizeof(T),
				std::span{R::members},

				//Default Contructor
				[](void* Dst)
				{
					new (Dst) T();
				},

				//Copy Contructor
				[](void* Dst, const void* Src)
				{
					new (Dst) T(*reinterpret_cast<const T*>(Src));
				},

				//Move Contructor
				[](void* Dst, void* Src)
				{
					new (Dst) T(std::move(*reinterpret_cast<T*>(Src)));
				},

				//Destructor
				[](void* Obj)
				{
					reinterpret_cast<T*>(Obj)->~T();
				}
			};
		}
	};

public:
	ReflectionRegistry();
	~ReflectionRegistry();

	void RegisterType(const ReflectionRegistry::TypeInfo& Info);

	const ReflectionRegistry::TypeInfo& GetInfo(TypeIdHash Hash);

	template<typename T>
	const ReflectionRegistry::TypeInfo GetInfo()
	{
		auto it = mData.find(GetTypeId<T>());
		if (it == mData.end())
		{
			_LOG_CORE_WARNING("Trying to get a non registered TypeInfo: {}", typeid(T).name());
			return {};
		}

		return it->second;
	}

	Vector<ReflectionRegistry::TypeInfo> GetAllInfos();

private:
	UnorderedMap<TypeIdHash, TypeInfo> mData;
};

#define REFLECT_MEMBERS(REFLECT, TYPE, ...) \
	static constexpr auto members = std::to_array<ReflectionRegistry::MemberInfo>({__VA_ARGS__ }); \
	static constexpr ReflectionRegistry::TypeInfo typeInfo = ReflectionRegistry::TypeInfo::MakeInfo<TYPE, REFLECT>(#TYPE)

#define REFLECT_MEMBER(OWNER, MEMBER) \
	ReflectionRegistry::MemberInfo::MakeInfo(#MEMBER, GetTypeId<decltype(OWNER::MEMBER)>(), offsetof(OWNER, MEMBER), sizeof(decltype(OWNER::MEMBER)))
