#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>

#include "IModule.h"
#include "EventDispatcher/Notifier.h"
#include <Windows.h>

struct ModuleInfo
{
	IModule* module;
	HMODULE library; //TODO: make cross platform compatible
	String name;
};

/*
* Engine subsystem
*/
class ModuleManager
{
	friend struct subsystem::SubsystemDescriptor;
public:

	ENGINE_API bool LoadModule(const String& ModuleName);
	ENGINE_API bool UnLoadModule(const String& ModuleName);

	ENGINE_API const ModuleInfo* TryGetModule(const String& ModuleName) const;
	ENGINE_API const IModule* GetModule(const String& ModuleName) const;
	ENGINE_API HMODULE GetHModule(const String& ModuleName) const;

	static MultiNotifierArgs<String> OnModuleUnload;

private:
	ModuleManager();
	~ModuleManager();

private:
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