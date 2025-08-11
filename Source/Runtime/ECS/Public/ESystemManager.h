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

	void RegisterSystem(Func<void(EWorld&)> SystemFunction, const String& Name, EPipeline Pipeline)
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

		mRegisteredSystemsName.insert({ Name, system });
		mRegisteredSystemsPipeline[Pipeline].push_back(system);
	}

	void UnRegisterSystems()
	{
		mRegisteredSystemsName.clear();
		mRegisteredSystemsPipeline.clear();
	}

private:
	ESystemManager();
	~ESystemManager() override;
private:
	ENameSystemMap mRegisteredSystemsName;
	EPipelineSystemMap mRegisteredSystemsPipeline;
};