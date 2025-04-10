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


extern SINGLETON_API std::unordered_map<std::type_index, void*> __sSingletons;

template<class DerivedClass>
class Singleton
{
public:
	static void Create()
	{
		std::call_once(initFlag, [] {
			auto it = __sSingletons.find(typeid(DerivedClass));
			assert(it == __sSingletons.end());//TODO: look in to why static loading casues assert for sandbox, why doesnt it share memory

			DerivedClass* instance = new DerivedClass();
			__sSingletons[typeid(DerivedClass)] = (void*)instance;
			});
	}

	static void Shutdown()
	{
		auto it = __sSingletons.find(typeid(DerivedClass));
		assert(it != __sSingletons.end());
		if (it != __sSingletons.end())
		{
			delete static_cast<DerivedClass*>(it->second);
			it->second = nullptr;
		}
		__sSingletons.erase(typeid(DerivedClass));
	}

	static DerivedClass* GetInstance()
	{
		auto it = __sSingletons.find(typeid(DerivedClass));
		if (it != __sSingletons.end())
			return static_cast<DerivedClass*>(it->second);
		else
			return nullptr;
	}



protected:
	Singleton() = default;
	virtual ~Singleton() = default;
private:
	static std::once_flag initFlag;
};

template<class DerivedClass>
std::once_flag Singleton<DerivedClass>::initFlag;

namespace _Singleton
{
	inline void CheckAllHasShutdown()
	{
		assert(__sSingletons.size() <= 0);
	}
}