#pragma once
#include <assert.h>
#include <unordered_map>
#include <typeindex>
#include <mutex>

#ifdef _WIN32

#ifdef SINGLETON_EXPORTS
#define SINGLETON_API __declspec(dllexport)
#else // SINGLETON_EXPORTS
#define SINGLETON_API __declspec(dllimport)
#endif // SINGLETON_EXPORTS

#else // _WIN32
#define SINGLETON_API 
#endif // _WIN32


namespace __SingletonRegistryFunctions
{
	SINGLETON_API void Register(const std::type_index& Type, void* Instance);
	SINGLETON_API void* Get(const std::type_index& Type);
	SINGLETON_API void Unregister(const std::type_index& Type);
	SINGLETON_API void EnsureAllShutdown();
}

template<class DerivedClass>
class EngineSubsystem
{
public:
	static void Create()
	{
		static std::once_flag flag;
		std::call_once(flag, []
			{
				__SingletonRegistryFunctions::Register(typeid(DerivedClass), new DerivedClass());
			});
	}

	static void Shutdown()
	{
		auto* instance = Get();
		delete instance;
		__SingletonRegistryFunctions::Unregister(typeid(DerivedClass));
	}

	static DerivedClass* Get()
	{
		return static_cast<DerivedClass*>(__SingletonRegistryFunctions::Get(typeid(DerivedClass)));
	}

	static DerivedClass* GetRef()
	{
		DerivedClass* ptr = Get();
		assert(ptr);
		return *ptr;
	}

protected:
	EngineSubsystem() = default;
	virtual ~EngineSubsystem() = default;
private:
};