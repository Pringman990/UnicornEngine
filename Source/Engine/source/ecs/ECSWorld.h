#pragma once
#include "ECSCommon.h"


namespace ecs
{

	struct Transform
	{
		int a =10;
	};

	struct Mesh
	{
		int b = 10;
		std::vector<int> vector;
	};

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
		void AddComponent(Entity anEntity, T aComponent);

		template<typename T>
		void RemoveComponent(Entity anEntity, T aComponent);

		template<typename T>
		T* GetComponent(Entity anEntity);

		template<typename T>
		bool HasComponent(Entity anEntity);

		template<typename... Components>
		void BindSystem(System<Components...> aSystem, Pipeline aPipeline = OnUpdate);

		void ProcessSystems();

	private:
		template<typename... Components>
		Signature CalulateSignature();
	private:
		Entity mNextEntity = 0;
		SystemMap mSystems;

		std::unordered_map<ComponentType, std::any> mComponentArrays;
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

	template<typename T>
	inline void World::AddComponent(Entity anEntity, T aComponent)
	{
		auto type = ComponentType(typeid(T));
		std::vector<T>* componentVector = std::any_cast<std::vector<T>>(&mComponentArrays[type]);
		if (componentVector == nullptr)
		{
			mComponentArrays[type] = std::vector<T>();
			componentVector = std::any_cast<std::vector<T>>(&mComponentArrays[type]);
		}
		componentVector->push_back(aComponent);
		mEntityToIndex[anEntity][type] = componentVector->size() - 1;
		mIndexToEntity[type][componentVector->size() - 1] = anEntity;
	}

	template<typename T>
	inline void World::RemoveComponent(Entity anEntity, T aComponent)
	{
		auto type = ComponentType(typeid(T));
		std::vector<T>* componentVector = mComponentArrays[type];

		size_t componentIndex = mEntityToIndex[anEntity][type];

		Entity otherEntity = mIndexToEntity[type][componentVector->size()];
		mEntityToIndex[otherEntity][type] = componentIndex;

		componentVector->swap(componentIndex, componentVector->size());
		componentVector->pop_back();
	}

	template<typename T>
	inline T* World::GetComponent(Entity anEntity)
	{
		auto type = ComponentType(typeid(T));
		size_t componentIndex = mEntityToIndex[anEntity][type];
		std::vector<T>* componentVector = std::any_cast<std::vector<T>>(&mComponentArrays[type]);
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