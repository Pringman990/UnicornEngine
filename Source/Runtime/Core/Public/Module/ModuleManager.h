#pragma once
#include <EngineSubsystem.h>
#include "IModule.h"
#include <Windows.h>

class ModuleManager : public EngineSubsystem<ModuleManager>
{
	friend class EngineSubsystem<ModuleManager>;
public:

	bool LoadModule(const String& ModuleName);
	bool UnLoadModule(const String& ModuleName);

	const IModule* GetModule(const String& ModuleName) const;
	HMODULE GetHModule(const String& ModuleName) const;

private:
	ModuleManager();
	~ModuleManager();

private:
	struct ModuleInfo
	{
		IModule* module;
		HMODULE library; //TODO: make cross platform compatible
	};

	UnorderedMap<String, ModuleInfo> mLoadedModules;
};

#define IMPLEMENT_MODULE(MODULE_NAME) \
	extern "C" __declspec(dllexport) IModule* CreateModule() \
	{ \
		return new MODULE_NAME(); \
	}

#define IMPLEMENT_MAIN_GAME_MODULE(MODULE_NAME) \
class MODULE_NAME : public IModule \
{ \
public: \
	virtual ~MODULE_NAME() override {}; \
	virtual void Startup() override {}; \
	virtual void Shutdown() override {}; \
}; \
	IMPLEMENT_MODULE(MODULE_NAME)