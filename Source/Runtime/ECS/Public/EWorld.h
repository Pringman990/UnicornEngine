#pragma once
#include "ECSDefines.h"
#include <Core.h>
#include "ECommon.h"
#include "EComponentRegistry.h"
#include "EComponentAllocator.h"
#include "Reflection/ReflectionRegistry.h"

template<typename T, typename... List>
struct IndexHelper;

template<typename T, typename First, typename... Rest>
struct IndexHelper<T, First, Rest...>
{
	static constexpr uint32 value = std::is_same<T, First>::value ? 0 : 1 + IndexHelper<T, Rest...>::value;
};

template<typename T>
struct IndexHelper<T>
{
	static constexpr uint32 value = 0xFFFFFFFF; // or assert fail if desired
};

template<typename... Components>
class EQueryResult
{
	friend class EWorld;
public:
	uint32 GetCount() const { return mCount; }

	template<typename T>
	T& Get(uint32 Index)
	{
		return *reinterpret_cast<T*>(mComponentArrays[IndexOf<T>()][Index]);
	}

private:
	template<typename T>
	static constexpr uint32 IndexOf()
	{
		return IndexHelper<T, Components...>::value;
	}
private:
	uint32 mCount = 0;
	Array<Vector<void*>, sizeof...(Components)> mComponentArrays;
};

class EWorld final
{

public:
	ECS_API EWorld();
	ECS_API ~EWorld();

	ECS_API EEntity CreateEntity();
	ECS_API EEntity CreateEntity(const UniqueID128& UUID);

	//template<typename T>
	//T* AddComponent(EEntity Entity, T&& Component);

	template<refl::IsComponent T>
	T* AddComponent(EEntity Entity);

	ECS_API void* AddComponent(EEntity Entity, const UniqueID128& UUID);

	template<refl::IsComponent T>
	void RemoveComponent(EEntity Entity);

	ECS_API void RemoveComponent(EEntity Entity, const UniqueID128& UUID);

	template<refl::IsComponent T>
	T* GetComponent(EEntity Entity)
	{
		ComponentStore* store = GetStore<T>();
		if (!store)
		{
			LOG_WARNING("Trying to get component of type '{}' but that component has no store", typeid(T).name());
			return nullptr;
		}

		if (!store->indicies.contains(Entity))
		{
			//LOG_WARNING("Trying to get component on entity '{}' but it does not have one", Entity.ToString());
			return nullptr;
		}

		return reinterpret_cast<T*>(store->allocator.Get(store->indicies[Entity]));
	}

	template<refl::IsComponent... Components>
	EQueryResult<Components...> Query()
	{
		EQueryResult<Components...> result;

		auto stores = Array<ComponentStore*, sizeof...(Components)>{
			GetStore<Components>()...
		};

		for (ComponentStore* s : stores)
		{
			if (!s)
			{
				//LOG_WARNING("Trying to query components but a component store did not exist");
				return result;
			}
		}

		ComponentStore* smallest = stores[0];
		for (ComponentStore* s : stores)
		{
			if (s->allocator.GetCount() < smallest->allocator.GetCount())
				smallest = s;
		}	

		for (uint32 i = 0; i < smallest->allocator.GetCount(); i++)
		{
			EEntity e = smallest->entities[i];

			bool entityHasAllComponents = true;
			for (ComponentStore* s : stores)
			{
				if (!s->HasEntity(e))
				{
					entityHasAllComponents = false;
					break;
				} 
			}

			if (entityHasAllComponents == false)
			{
				continue;
			}
			
			(result.mComponentArrays[result.IndexOf<Components>()]
				.push_back(stores[result.IndexOf<Components>()]->Get<Components>(stores[result.IndexOf<Components>()]->indicies[e])), ...);

			result.mCount++;
		}

		return result;
	}

	const Vector<EEntity>& GetAllEntities() const { return mEntities; }

	UnorderedMap<UniqueID128, ComponentStore>& ComponentAllStores() { return mComponentStores; };

private:
	template<refl::IsComponent T>
	ComponentStore* GetStore()
	{
		const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull<T>();
		if (!type)
		{
			LOG_WARNING("Can't get ComponentStore of null type, {}", typeid(T).name());
			return nullptr;
		}

		auto it = mComponentStores.find(type->uuid);
		if (it == mComponentStores.end())
		{
			return nullptr;
		}
		return &it->second;
	}

private:
	UnorderedMap<UniqueID128, ComponentStore> mComponentStores;
	Vector<EEntity> mEntities;
};

template<refl::IsComponent T>
inline T* EWorld::AddComponent(EEntity Entity)
{
	const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull<T>();
	if (!type)
	{
		LOG_WARNING("Can't get ComponentStore of null type, {}", typeid(T).name());
		return nullptr;
	}

	return static_cast<T*>(AddComponent(Entity, type->uuid));
}

template<refl::IsComponent T>
inline void EWorld::RemoveComponent(EEntity Entity)
{
	const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull<T>();
	if (!type)
	{
		LOG_WARNING("Can't get ComponentStore of null type, {}", typeid(T).name());
		return;
	}

	RemoveComponent(Entity, type->uuid);
}
