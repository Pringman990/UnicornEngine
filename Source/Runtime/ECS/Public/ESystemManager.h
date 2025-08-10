#pragma once
#include <Core.h>
#include <EngineSubsystem.h>
#include "EWorld.h"

class ESystemManager : public EngineSubsystem<ESystemManager>
{
	friend class EngineSubsystem<ESystemManager>;
public:

	void RunLoad(EWorld& World);
	void RunUpdate(EWorld& World);
	void RunRender(EWorld& World);

	template<typename... Components>
	void RegisterSystem(Func<void(EWorld&, EEntity, Components&...)> SystemFunction, const String& Name, EPipeline Pipeline)
	{
		if (mRegisteredSystemsName.contains(Name))
		{
			_LOG_CORE_ERROR("System already exist: {}", Name);
			return;
		}

		ESystem system;
		system.name = Name;
		system.function = SystemFunction;
		system.pipeline = Pipeline;
		//system.signature = internal::EComponentRegistry::Get().CalulateSignature<Components...>();

		mRegisteredSystemsName.insert({ Name, system });
		mRegisteredSystemsPipeline[Pipeline].push_back(system);
	}

private:
	ESystemManager();
	~ESystemManager() override;
private:
	ENameSystemMap mRegisteredSystemsName;
	EPipelineSystemMap mRegisteredSystemsPipeline;
};