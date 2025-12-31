#include "ESystemManager.h"

ESystemManager::ESystemManager()
{
}

ESystemManager::~ESystemManager()
{
}

ESystemManager* ESystemManager::Instance()
{
	static ESystemManager* instance = new ESystemManager();
	return instance;
}

void ESystemManager::RunLoad(EWorld& World, uint32 ExecutionPhase)
{
#ifdef _DEBUG
	mSystemsDebugInfo.clear();
#endif // _DEBUG


	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemLoad])
	{
		if(CanRun(sys, ExecutionPhase))
			sys->function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPostLoad])
	{
		if (CanRun(sys, ExecutionPhase))
			sys->function(World);
	}
}

void ESystemManager::RunUpdate(EWorld& World, uint32 ExecutionPhase)
{
	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPreUpdate])
	{
		if (CanRun(sys, ExecutionPhase))
			sys->function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemUpdate])
	{
		if (CanRun(sys, ExecutionPhase))
			sys->function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPostUpdate])
	{
		if (CanRun(sys, ExecutionPhase))
			sys->function(World);
	}
}