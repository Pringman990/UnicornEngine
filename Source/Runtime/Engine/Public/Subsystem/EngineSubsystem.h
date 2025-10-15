#pragma once
#include <Core.h>
#include "EngineDefines.h"
#include "Logger/Logger.h"


template<typename Derived>
class EngineSubsystem; // forward declaration

namespace subsystem
{
	struct SubsystemDescriptor
	{
		std::string_view name;
		std::span<const std::string_view> dependencies;
		size_t size;
		ConstructFunc constructor = nullptr;
		DestroyFunc destructor = nullptr;

		template<typename T, size_t N>
		inline static SubsystemDescriptor MakeDescriptor(std::string_view Name, const std::array<std::string_view, N>& Dep)
		{
			return {
				Name,
				std::span<const std::string_view>(Dep),
				sizeof(T),
				
				//Default Contructor
				[](void* Dst)
				{
					new (Dst) T();
				},

				//Destructor
				[](void* Obj)
				{
					reinterpret_cast<T*>(Obj)->~T();
				}
			};
		}
	};
}

namespace __Subsystem
{
	struct Singleton
	{
		void* instance = nullptr;
		const subsystem::SubsystemDescriptor* desc = nullptr;
	};

	ENGINE_API void CreateAllSystems();
	ENGINE_API void Register(std::type_index Type, const subsystem::SubsystemDescriptor* Desc);
	ENGINE_API void* Get(const std::type_index& Type);
	ENGINE_API UnorderedMap<std::type_index, Singleton>& GetAll();
	ENGINE_API void Unregister(const std::type_index& Type);
	ENGINE_API void EnsureAllShutdown();

	template <typename... Deps>
	static constexpr auto MakeDeps(Deps&&... deps)
	{
		if constexpr (sizeof...(deps) == 0)
		{
			return std::array<std::string_view, 0>{};
		}
		else
		{
			return std::to_array<std::string_view>({ std::forward<Deps>(deps)... });
		}
	}
}

class SubsystemManager
{
public:

	static void Init()
	{
		__Subsystem::CreateAllSystems();
	}

	template<typename T>
	static T* Get()
	{
		return static_cast<T*>(__Subsystem::Get(typeid(T)));
	}

	template<typename T>
	static void RequestDelete()
	{
		__Subsystem::Unregister(typeid(T));
	}

	static void DestroyAll()
	{
		for (auto& sys : __Subsystem::GetAll())
		{
			__Subsystem::Unregister(sys.first);
		}
		__Subsystem::GetAll().clear();
	}

private:
	SubsystemManager() {};
	~SubsystemManager() {};
};

#define REGISTER_ENGINE_SUBSYSTEM(CLASS, ...) \
namespace { \
	struct __Engine_Subsystem_Auto_Register { \
		subsystem::SubsystemDescriptor _descriptor = subsystem::SubsystemDescriptor::MakeDescriptor<CLASS>(#CLASS, __Subsystem::MakeDeps(#__VA_ARGS__)); \
		__Engine_Subsystem_Auto_Register() { \
			__Subsystem::Register(typeid(CLASS), &_descriptor); \
		} \
	}; \
	static const __Engine_Subsystem_Auto_Register TRUNCATE(__Engine_Subsystem_Auto_Register_, __LINE__); \
}