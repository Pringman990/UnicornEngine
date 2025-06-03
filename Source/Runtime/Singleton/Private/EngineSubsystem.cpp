#include "EngineSubsystem.h"
#include <iostream>
namespace 
{
	void __declspec(dllexport) i()
	{
	
	}
	SINGLETON_API std::unordered_map<std::type_index, void*> __sSingletons;
	static std::mutex sMutex;
}

namespace __SingletonRegistryFunctions
{
	void Register(const std::type_index& Type, void* Instance)
	{
		std::scoped_lock lock(sMutex);
		assert(__sSingletons.contains(Type) == false);
		__sSingletons[Type] = Instance;

#ifdef _DEBUG
		std::cout << "Register engine subsystem: " << Type.name() << "\n";
#endif // _DEBUG

	}

	void* Get(const std::type_index& Type)
	{
		std::scoped_lock lock(sMutex);
		auto it = __sSingletons.find(Type);
		return it != __sSingletons.end() ? it->second : nullptr;
	}

	void Unregister(const std::type_index& Type)
	{
		std::scoped_lock lock(sMutex);
		__sSingletons.erase(Type);
	}

	void EnsureAllShutdown()
	{
		assert(__sSingletons.size() <= 0);
	}
}
