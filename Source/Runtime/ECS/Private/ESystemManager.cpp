#include "ESystemManager.h"

ESystemManager* ESystemManager::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(ESystemManager)

ESystemManager::ESystemManager()
{
#ifdef _DEBUG
	ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
	sInstance = this;
}

ESystemManager::~ESystemManager()
{
	if (sInstance == this)
		sInstance = nullptr;
}

ESystemManager* ESystemManager::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
}

void ESystemManager::RunLoad(EWorld& World)
{
#ifdef _DEBUG
	mSystemsDebugInfo.clear();
#endif // _DEBUG


	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemLoad])
	{
		sys.function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPostLoad])
	{
		sys.function(World);
	}
}

void ESystemManager::RunUpdate(EWorld& World)
{
	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPreUpdate])
	{
		sys.function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemUpdate])
	{
		sys.function(World);
	}

	for (auto& sys : mRegisteredSystemsPipeline[EPipeline::ESystemPostUpdate])
	{
		sys.function(World);
	}
}