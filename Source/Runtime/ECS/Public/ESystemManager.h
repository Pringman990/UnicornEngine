#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include "EWorld.h"

struct ESystemDebugInfo
{
	uint32 entityCount = 0;
	float frameRunTime = 0; //In seconds
};

/*
* Engine subsystem
*/
class ESystemManager
{
	friend struct subsystem::SubsystemDescriptor;
public:

	void RunLoad(EWorld& World);
	void RunUpdate(EWorld& World);

	void RegisterSystem(Func<void(EWorld&)> SystemFunction, const String& Name, EPipeline Pipeline)
	{
		if (mRegisteredSystemsName.contains(Name))
		{
			LOG_ERROR("System already exist: {}", Name);
			return;
		}

		ESystem system;
		system.name = Name;
		system.function = SystemFunction;
		system.pipeline = Pipeline;

		mRegisteredSystemsName.insert({ Name, system });
		mRegisteredSystemsPipeline[Pipeline].push_back(system);
	}

	const ENameSystemMap& GetRegisteredSystems() const { return mRegisteredSystemsName; }

	void UnRegisterSystems()
	{
		mRegisteredSystemsName.clear();
		mRegisteredSystemsPipeline.clear();
	}

#ifdef _DEBUG
	void RegisterFrameDebugInfo(const String& SystemName, const ESystemDebugInfo& Info)
	{
		if (!mRegisteredSystemsName.contains(SystemName))
		{
			LOG_ERROR("Can't add system debug info on non registered ecs system: {}", SystemName);
			return;
		}

		mSystemsDebugInfo[SystemName] = Info;
	}

	const UnorderedMap<String, ESystemDebugInfo>& GetSystemsDebugInfo() const { return mSystemsDebugInfo; }
#endif // _DEBUG


private:
	ESystemManager();
	~ESystemManager();
private:
	ENameSystemMap mRegisteredSystemsName;
	EPipelineSystemMap mRegisteredSystemsPipeline;

#ifdef _DEBUG
	UnorderedMap<String, ESystemDebugInfo> mSystemsDebugInfo;
#endif // _DEBUG

};

#define GET_ESYSTEMMANAGER() SubsystemManager::Get<ESystemManager>()