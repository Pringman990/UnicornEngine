#pragma once
#include <Singleton.h>

#include <unordered_map>
#include <typeindex>
#include <memory>

#include "IAssetManager.h"

class AssetRegistry : public Singleton<AssetRegistry>
{
public:
	template<typename _Type>
	void RegisterManager(_Type* aManager)
	{
		std::type_index type = typeid(_Type);
		mManagers[type] = aManager;
	}

	template<typename _Type>
	void RegisterManager(_Type aType, _Type* aManager)
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
			return static_cast<_Manager*>(it->second);
		}
		return nullptr;
	}
public: 
	template<typename _AssetType>
	_AssetType* GetAssetByPath(const std::string& aPath)
	{
		for (auto& [typeindex, manager] : mManagers)
		{
			if (std::is_base_of<decltype(typeindex), _AssetType>::value)
			{
				auto assetManager = std::static_pointer_cast<IAssetManager<decltype(typeindex)>>(manager);
				return static_cast<_AssetType>(assetManager->LoadAsset());
			}
		}
		return nullptr;
	}

private:
	friend class Singleton<AssetRegistry>;
	AssetRegistry();
	~AssetRegistry();
private:

	std::unordered_map<std::type_index, void*> mManagers;
};

#define GET_ASSET_MANAGER(TYPE) \
	AssetRegistry::GetInstance()->GetManager<TYPE>();

