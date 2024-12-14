#include "ResourceRegistry.h"

ResourceRegistry::ResourceRegistry()
{
}

ResourceRegistry::~ResourceRegistry()
{
	for (auto& manager : mManagers)
	{
		delete manager.second;
		manager.second = nullptr;
	}
	mManagers.clear();
}
