#pragma once
#include <Core.h>
#include <EngineSubsystem.h>
#include "EWorld.h"

class ESystemManager : public EngineSubsystem<ESystemManager>
{
	friend class EngineSubsystem<ESystemManager>;
public:

	void RunLoad(EWorld& aWorld);
	void RunUpdate(EWorld& aWorld);
	void RunRender(EWorld& aWorld);

	template<typename... Components>
	void RegisterSystem(Func<void(EWorld&, EEntity, Components&...)> aSystemFunction, const String& aName, EPipeline aPipeline)
	{
		if (mRegisteredSystemsName.contains(aName))
		{
			_LOG_CORE_ERROR("System already exist: {}", aName);
			return;
		}

		ESystem system;
		system.name = aName;
		system.function = aSystemFunction;
		system.pipeline = aPipeline;
		//system.signature = internal::EComponentRegistry::Get().CalulateSignature<Components...>();

		mRegisteredSystemsName.insert({ aName, system });
		mRegisteredSystemsPipeline[aPipeline].push_back(system);
	}

private:
	ESystemManager();
	~ESystemManager() override;
private:
	ENameSystemMap mRegisteredSystemsName;
	EPipelineSystemMap mRegisteredSystemsPipeline;
};