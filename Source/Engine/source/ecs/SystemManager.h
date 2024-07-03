#pragma once
#include "ECSCommon.h"

//class Engine;

namespace ecs
{
	class SystemManager
	{
		friend class Engine;
	public:
		template<typename... Components>
		void RegisterSystem(SystemFunctionT<Components...> aSystem, const std::string& aName, Pipeline aPipeline = OnUpdate);

		template<typename... Components>
		void _RegisterEngineSystem(SystemFunctionT<Components...> aSystem, const std::string& aName, Pipeline aPipeline = OnUpdate);

		std::shared_ptr<System> GetSystemByName(const std::string& aName);

		void RunUpdateEngineSystems();
		void RunRenderEngineSystems();

	private:
		SystemManager() {};
		~SystemManager() {};
	private:
		NameSystemMap mSystems;
		PipelineSystemMap mEngineSystems;
	};

	template<typename ...Components>
	inline void SystemManager::RegisterSystem(SystemFunctionT<Components...> aSystem, const std::string& aName, Pipeline aPipeline)
	{
		auto it = mSystems.find(aName);
		if (it != mSystems.end())
			assert(false); //System is already registered

		System system;
		system.name = aName;
		system.pipeline = aPipeline;
		system.signature = internal::ComponentRegistry::GetInstance().CalulateSignature<Components...>();
		system.function = ([=](World& world, Entity entity)
			{
				aSystem(world, entity, *world.GetComponent<Components>(entity)...);
			});

		mSystems[aName] = system;
	}
	template<typename ...Components>
	inline void SystemManager::_RegisterEngineSystem(SystemFunctionT<Components...> aSystem, const std::string& aName, Pipeline aPipeline)
	{
		auto it = mSystems.find(aName); 
		if (it != mSystems.end())
			assert(false); //System is already registered
		
		System system;
		system.name = aName;
		system.pipeline = aPipeline;
		system.signature = internal::ComponentRegistry::GetInstance().CalulateSignature<Components...>();
		system.function = ([=](World& world, Entity entity)
			{
				aSystem(world, entity, *world.GetComponent<Components>(entity)...);
			});

		mSystems[aName] = system;
		mEngineSystems[aPipeline].push_back(std::make_shared<System>(mSystems[aName]));
	}
}