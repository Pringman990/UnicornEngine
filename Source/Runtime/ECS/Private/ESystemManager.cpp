#include "ESystemManager.h"

ESystemManager::ESystemManager()
{
}

ESystemManager::~ESystemManager()
{
}

void ESystemManager::RunLoad(EWorld& World)
{
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