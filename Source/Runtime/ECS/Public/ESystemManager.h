#pragma once
#include "ECSDefines.h"
#include <Core.h>
#include "EWorld.h"

struct ECS_API ESystemDebugInfo
{
	uint32 entityCount = 0;
	float frameRunTime = 0; //In seconds
};

class ESystemManager
{
public:

	ECS_API static ESystemManager* Instance();

	ECS_API void RunLoad(EWorld& World, uint32 ExecutionPhase);
	ECS_API void RunUpdate(EWorld& World, uint32 ExecutionPhase);

	void RegisterSystem(Func<void(EWorld&)> SystemFunction, UniqueID128 UUID, const String& Name, EPipeline Pipeline, uint32 ExecutionPhase)
	{
		if (mSystems.contains(UUID) || mRegisteredSystemsName.contains(Name))
		{
			//LOG_ERROR("System already exist: {}, {}", UUID, Name);
			return;
		}

		OwnedPtr<ESystem> system = MakeOwned<ESystem>();
		system->name = Name;
		system->function = SystemFunction;
		system->pipeline = Pipeline;
		system->executionPhase = ExecutionPhase;

		mSystems.insert({UUID, std::move(system)});
		auto sysPtr = mSystems[UUID].get();
		mRegisteredSystemsName.insert({ Name, sysPtr });
		mRegisteredSystemsPipeline[Pipeline].push_back(sysPtr);
	}

	void UnregisterSystem(UniqueID128 UUID)
	{
		auto it = mSystems.find(UUID);
		if (it != mSystems.end())
		{
			mRegisteredSystemsName.erase(it->second->name);
			EraseItemFromVector(mRegisteredSystemsPipeline[it->second->pipeline], it->second.get());
			mSystems.erase(UUID);
		}
	}

	const UnorderedMap<String, ESystem*>& GetRegisteredSystems() const { return mRegisteredSystemsName; }

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
	ECS_API ESystemManager();
	ECS_API ~ESystemManager();

	bool CanRun(ESystem* System, uint32 ExecutionPhase)
	{
		return (System->executionPhase & ExecutionPhase) == System->executionPhase;
	}
private:

	UnorderedMap<UniqueID128, OwnedPtr<ESystem>> mSystems;

	UnorderedMap<String, ESystem*> mRegisteredSystemsName;
	UnorderedMap<EPipeline, Vector<ESystem*>> mRegisteredSystemsPipeline;

#ifdef _DEBUG
	UnorderedMap<String, ESystemDebugInfo> mSystemsDebugInfo;
#endif // _DEBUG

};

#define REGISTER_ESYSTEM(NAME, UUID, PIPELINE, EXECUTIONPHASE, SYSTEM) \
	namespace \
	{ \
		struct TRUNCATE(ESystem_Register_, __LINE__) \
		{ \
			TRUNCATE(ESystem_Register_, __LINE__)() \
			{ \
				ESystemManager::Instance()->RegisterSystem(&SYSTEM, UniqueID128(UUID), NAME, PIPELINE, EXECUTIONPHASE); \
			} \
			~TRUNCATE(ESystem_Register_, __LINE__)() \
			{ \
				ESystemManager::Instance()->UnregisterSystem(UniqueID128(UUID)); \
			} \
		}; \
	} \
	static const TRUNCATE(ESystem_Register_, __LINE__) TRUNCATE(Static_ESystem_Register_, __LINE__);
