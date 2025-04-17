#pragma once
#include <Singleton.h>
#include "IModule.h"
#include <Windows.h>

class ModuleManager : public Singleton<ModuleManager>
{
	friend class Singleton<ModuleManager>;
public:

	bool LoadModule(const std::string& aModuleName);
	bool UnLoadModule(const std::string& aModuleName);

	const IModule* GetModule(const std::string& aModuleName) const;

private:
	ModuleManager();
	~ModuleManager();

private:
	struct ModuleInfo
	{
		IModule* module;
		HMODULE library;
	};

	std::unordered_map<std::string, ModuleInfo> mLoadedModules;
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