#pragma once
#include "ECSCommon.h"

namespace ecs
{

	class World final
	{
		template<typename... Components>
		using System = std::function<void(World&, Entity, Components&...)>;

		using SystemVector = std::vector<std::function<void(World&, Entity)>>;
		using SystemMap = std::unordered_map<Pipeline, SystemVector>;

	public:
		World();
		~World();

		Entity CreateEntity();

		template<typename T>
		void AddComponent(Entity anEntity, T aComponent);

		template<typename... Components>
		void BindSystem(System<Components...> aSystem, Pipeline aPipeline);

		void ProcessSystems();

	private:

	private:
		Entity mNextEntity = 0;

		std::unordered_map<ArchetypeMask, Archetype> mArchetypes;


		SystemMap mSystems;
	};


	//class World final
	//{
	//	using SystemFunction = std::function<void(World&, Entity, std::tuple<>)>;
	//	struct SystemWrapper {
	//		ArchetypeMask mask;
	//		SystemFunction func;
	//	};

	//public:
	//	World();
	//	~World();

	//	Entity CreateEntity();

	//	template<typename T>
	//	void AddComponent(Entity anEntity, T aComponent);

	//	template<typename T>
	//	void RemoveComponent(Entity anEntity);

	//	template<typename T>
	//	bool HasComponent(Entity anEntity);

	//	template<typename T>
	//	T GetComponent(Entity anEntity);

	//	template<typename... Components>
	//	void BindSystem(SystemFunction aSystem, Pipeline aPipeline);

	//	void ProcessSystems();

	//private:
	//	bool EntityMatchesArchetype(Entity anEntity, ArchetypeMask aMask);
	//	void FindNewArchetypeForEntity(Entity anEntity, ArchetypeMask aMask);

	//	template<typename T>
	//	ComponentStorage<T>* GetComponentStorage();

	//	template<typename... Components>
	//	std::tuple<Components&...> GetComponentsForEntity(Entity entity);
	//private:
	//	Entity mNextEntity = 0;
	//	std::unordered_map<Entity, ComponentMask> mEntityComponentMasks;
	//	std::unordered_map<std::type_index, void*> mRegisteredComponents;
	//	std::unordered_map<ArchetypeMask, std::unordered_set<Entity>> mArchetypes;

	//	std::unordered_map<Pipeline, std::vector<SystemWrapper>> mSystems;
	//};



}