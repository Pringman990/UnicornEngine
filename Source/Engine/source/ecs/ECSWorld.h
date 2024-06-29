#pragma once
#include "ECSCommon.h"


namespace ecs
{
	class World final
	{
		template<typename... Components>
		using System = std::function<void(World&, Entity, Components&...)>;

		using SystemVector = std::vector<std::pair<Signature, std::function<void(World&, Entity)>>>;
		using SystemMap = std::unordered_map<Pipeline, SystemVector>;

	public:
		World();
		~World();

		Entity CreateEntity();

		template<typename T>
		T* AddComponent(Entity anEntity, T aComponent);

		template<typename T>
		void RemoveComponent(Entity anEntity);

		template<typename T>
		T* GetComponent(Entity anEntity);

		std::unordered_map<ComponentType, void*> GetAllEntityComponents(Entity anEntity);

		template<typename T>
		bool HasComponent(Entity anEntity);

		template<typename... Components>
		void BindSystem(System<Components...> aSystem, Pipeline aPipeline = OnUpdate);

		void ProcessEngineRenderSystems();
		void ProcessUpdateSystems();
		void ProcessInitSystems();

	private:
		template<typename... Components>
		Signature CalulateSignature();

		ComponentMask GetEntityComponentMask(Entity anEntity);

	private:
		Entity mNextEntity = 0;
		SystemMap mSystems;

		/// <summary>
		/// The void pointer points to a std::vector of the component
		/// </summary>
		std::unordered_map<ComponentType, void*> mComponentArrays;
		std::unordered_map<ComponentType, std::unordered_map<size_t, Entity>> mIndexToEntity;
		std::unordered_map<Entity, std::unordered_map<ComponentType, size_t>> mEntityToIndex;
		std::unordered_map<ComponentMask, std::vector<Entity>> mMaskToEntity;
	};

	template<typename ...Components>
	inline void World::BindSystem(System<Components...> aSystem, Pipeline aPipeline)
	{
		Signature signature = CalulateSignature<Components...>();
		mSystems[aPipeline].emplace_back(signature,
			[=](World& world, Entity entity)
			{
				aSystem(world, entity, *world.GetComponent<Components>(entity)...);
			});
	}

	template<typename ...Components>
	inline ecs::Signature ecs::World::CalulateSignature()
	{
		Signature signature = 0;
		((signature |= (1 << internal::ComponentRegistry::GetInstance().TryGetMask<Components>())), ...);
		return signature;
	}


	inline ecs::ComponentMask ecs::World::GetEntityComponentMask(Entity anEntity)
	{
		ComponentMask mask = 0;
		auto it = mEntityToIndex.find(anEntity);
		if (it != mEntityToIndex.end())
		{
			for (auto& [componentType, componentIndex] : mEntityToIndex[anEntity])
			{
				((mask |= (1 << internal::ComponentRegistry::GetInstance().TryGetMask(componentType))));
			}
		}
		return mask;
	}

	template<typename T>
	inline T* World::AddComponent(Entity anEntity, T aComponent)
	{
		auto type = ComponentType(typeid(T));
		std::vector<T>* componentVector = static_cast<std::vector<T>*>(mComponentArrays[type]);
		if (componentVector == nullptr)
		{
			mComponentArrays[type] = new std::vector<T>();
			componentVector = static_cast<std::vector<T>*>(mComponentArrays[type]);
		}

		{
			ComponentMask mask = GetEntityComponentMask(anEntity);

			auto itToDelete = std::find(mMaskToEntity[mask].begin(), mMaskToEntity[mask].end(), anEntity);
			mMaskToEntity[mask].erase(itToDelete);
		}

		{
			componentVector->push_back(aComponent);
			mEntityToIndex[anEntity][type] = componentVector->size() - 1;
			mIndexToEntity[type][componentVector->size() - 1] = anEntity;

			ComponentMask mask = GetEntityComponentMask(anEntity);
			mMaskToEntity[mask].push_back(anEntity);
		}
		T* returnValue = &(*componentVector)[componentVector->size() - 1];
		return returnValue;
	}

	template<typename T>
	inline void World::RemoveComponent(Entity anEntity)
	{
		auto type = ComponentType(typeid(T));
		std::vector<T>* componentVector = static_cast<std::vector<T>*>(mComponentArrays[type]);

		size_t componentIndex = mEntityToIndex[anEntity][type];

		Entity otherEntity = mIndexToEntity[type][componentVector->size() - 1];
		size_t otherComponentIndexBefore = mEntityToIndex[otherEntity][type];
		mEntityToIndex[otherEntity][type] = componentIndex;
		
		{
			ComponentMask mask = GetEntityComponentMask(anEntity);

			auto itToDelete = std::find(mMaskToEntity[mask].begin(), mMaskToEntity[mask].end(), anEntity);
			mMaskToEntity[mask].erase(itToDelete);
		}
		
		mEntityToIndex[anEntity].erase(type);
		
		{
			ComponentMask mask = GetEntityComponentMask(anEntity);
			mMaskToEntity[mask].push_back(anEntity);
		}

		mIndexToEntity[type].erase(otherComponentIndexBefore);
		mIndexToEntity[type][componentIndex] = otherEntity;
		
		std::swap((*componentVector)[componentIndex], (*componentVector)[componentVector->size() - 1]);
		componentVector->pop_back();
	}

	template<typename T>
	inline T* World::GetComponent(Entity anEntity)
	{
		auto type = ComponentType(typeid(T));
		size_t componentIndex = mEntityToIndex[anEntity][type];
		std::vector<T>* componentVector = static_cast<std::vector<T>*>(mComponentArrays[type]);
		if (componentVector && componentIndex < componentVector->size())
		{
			return &(*componentVector)[componentIndex];
		}
		return nullptr;
	}

	template<typename T>
	inline bool World::HasComponent(Entity anEntity)
	{
		auto type = ComponentType(typeid(T));
		auto it = mEntityToIndex[anEntity].find(type);
		if (it != mEntityToIndex.end())
		{
			return true;
		}

		return false;
	}
}