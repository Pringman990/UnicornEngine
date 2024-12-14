#pragma once
#include <Singleton.h>

#include "IResourceManager.h"
#include <unordered_map>
#include <typeindex>

class ResourceRegistry : public Singleton<ResourceRegistry>
{
public:
	template<typename _Type>
	void RegisterManager(IResourceManager* aManager)
	{
		std::type_index type = typeid(_Type);
		mManagers[type] = aManager;
	}

	template<typename _Type>
	void RegisterManager(_Type aType, IResourceManager* aManager)
	{
		std::type_index type = typeid(aType);
		mManagers[type] = aManager;
	}

	template<typename _Type>
	void RegisterManager()
	{
		std::type_index type = typeid(_Type);
		_Type* manager = new _Type();
		mManagers[type] = manager;
	}

	template<typename _Manager>
	_Manager* GetManager()
	{
		std::type_index type = typeid(_Manager);
		auto it = mManagers.find(type);
		if (it != mManagers.end())
		{
			return dynamic_cast<_Manager*>(it->second);
		}
		return nullptr;
	}

private:
	friend class Singleton<ResourceRegistry>;
	ResourceRegistry();
	~ResourceRegistry();
private:
	 
	std::unordered_map<std::type_index, IResourceManager*> mManagers;
};

//CREATE_SINGLETON_STATIC(ResourceRegistry)

#define REGISTER_RESOURCEMANAGER(TYPE) \
	namespace { \
		struct __ResourceManagerRegister_##TYPE \
		{ \
			__ResourceManagerRegister_##TYPE() \
			{ \
				ResourceRegistry::GetInstance()->RegisterManager<TYPE>(); \
			} \
		}; \
		static __ResourceManagerRegister_##TYPE __sResourceManagerRegister_##TYPE; \
	}

#define GET_RESOURCE_MANAGER(TYPE) \
	ResourceRegistry::GetInstance()->GetManager<TYPE>();