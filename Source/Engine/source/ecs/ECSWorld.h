#pragma once
#include "ECSCommon.h"
#include <memory/ComponentAllocator.h>

namespace ecs
{
	class World final
	{

	public:
		World();
		~World();

		Entity CreateEntity();
		Entity CreateEntity(Entity aId);
		Entity CreateEntity(const std::string& aName);
		Entity CreateEntity(Entity aId, const std::string& aName);

		template<typename T>
		T* AddComponent(Entity anEntity, T aComponent);

		void AddComponent(Entity anEntity, const std::vector<char>& someData, const reflection::TypeInfo& aType);

		template<typename T>
		void RemoveComponent(Entity anEntity);

		template<typename T>
		T* GetComponent(Entity anEntity);

		std::unordered_map<ComponentType, void*> GetAllEntityComponents(Entity anEntity);

		template<typename T>
		bool HasComponent(Entity anEntity);

		void AddSystem(const std::string& aSystemName);

		void ProcessUpdateSystems();
		void ProcessInitSystems();

		const std::unordered_map<ComponentMask, std::vector<Entity>>& GetAllEntitiesWithMask() const;

		std::vector<Entity> GetAllEntites();

	private:
		ComponentMask GetEntityComponentMask(Entity anEntity);
	private:
		Entity mNextEntity;
		PipelineSystemMap mSystems;

		std::unordered_map<ComponentMask, std::vector<Entity>> mMaskToEntity;
		std::unordered_map<ComponentType, ComponentAllocator> mComponentAllocators;
		std::unordered_map<Entity, std::unordered_map<ComponentType, void*>> mEntityToComponent;
	};

	template<typename T>
	inline T* World::AddComponent(Entity anEntity, T aComponent)
	{
		{
			ComponentMask mask = GetEntityComponentMask(anEntity);

			auto itToDelete = std::find(mMaskToEntity[mask].begin(), mMaskToEntity[mask].end(), anEntity);
			if(itToDelete != mMaskToEntity[mask].end())
				mMaskToEntity[mask].erase(itToDelete);
		}

		std::type_index componentType = typeid(aComponent);
		reflection::TypeInfo typeinfo = Engine::GetReflectionRegistry().GetTypeInfo(componentType);

		auto it = mComponentAllocators.find(componentType);
		if (it == mComponentAllocators.end())
			mComponentAllocators[componentType] = ComponentAllocator(typeinfo);

		void* compPtr = mComponentAllocators[componentType].Allocate();
		T* co = new (compPtr) T(aComponent);

		mEntityToComponent[anEntity][componentType] = compPtr;

		ComponentMask mask = GetEntityComponentMask(anEntity);
		mMaskToEntity[mask].push_back(anEntity);

		return co;
	}

	template<typename T>
	inline void World::RemoveComponent(Entity anEntity)
	{
		{
			ComponentMask mask = GetEntityComponentMask(anEntity);

			auto itToDelete = std::find(mMaskToEntity[mask].begin(), mMaskToEntity[mask].end(), anEntity);
			mMaskToEntity[mask].erase(itToDelete);
		}

		std::type_index componentType = typeid(T);
		auto it = mComponentAllocators.find(componentType);
		if (it != mComponentAllocators.end())
		{
			mComponentAllocators[componentType].Deallocate(mEntityToComponent[anEntity][componentType]);
		}

		ComponentMask mask = GetEntityComponentMask(anEntity);
		mMaskToEntity[mask].push_back(anEntity);
	}

	template<typename T>
	inline T* World::GetComponent(Entity anEntity)
	{
#ifdef _DEBUG
		T* component = static_cast<T*>(mEntityToComponent[anEntity][typeid(T)]);
		return component;
#else
		return static_cast<T*>(mEntityToComponent[anEntity][typeid(T)]);
#endif // _DEBUG
	}

	template<typename T>
	inline bool World::HasComponent(Entity anEntity)
	{
		auto it = mEntityToComponent[anEntity].find(typeid(T));
		if (it != mEntityToComponent.end())
			return true;
		else
			return false;
	}

	inline ecs::ComponentMask ecs::World::GetEntityComponentMask(Entity anEntity)
	{
		ComponentMask mask = 0;
		auto it = mEntityToComponent.find(anEntity);
		if (it != mEntityToComponent.end())
		{
			for (auto& [componentType, componentIndex] : mEntityToComponent[anEntity])
			{
				((mask |= (1 << internal::ComponentRegistry::GetInstance().TryGetMask(componentType))));
			}
		}
		return mask;
	}
}