#pragma once
#include "IResourceManager.h"
#include <TUMap.h>
#include <FTypeIndex.h>

class ResourceRegistry
{
public:
	static ResourceRegistry& GetInstance()
	{
		if (!_sInstance)
			_sInstance = new ResourceRegistry();
		return *_sInstance;
	}

	template<typename _Type>
	void RegisterManager(IResourceManager* aManager)
	{
		FTypeIndex type = FTypeIndex::Create<_Type>();
		mManagers[type] = aManager;
	}

	template<typename _Type>
	void RegisterManager(_Type aType, IResourceManager* aManager)
	{
		FTypeIndex type = FTypeIndex::Create(aType);
		mManagers[type] = aManager;
	}

	template<typename _Manager>
	_Manager* GetManager()
	{
		FTypeIndex type = FTypeIndex::Create<_Manager>();
		auto it = mManagers.Find(type);
		if (it.IsValid())
		{
			return dynamic_cast<_Manager*>(it.Value());
		}
		return nullptr;
	}

private:
	ResourceRegistry() {};
	~ResourceRegistry() {};
private:
	static ResourceRegistry* _sInstance;

	TUMap<FTypeIndex, IResourceManager*> mManagers;
};

