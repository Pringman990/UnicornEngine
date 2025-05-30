#include "pch.h"
#include "Module/ModuleManager.h"

ModuleManager::ModuleManager()
{
}

ModuleManager::~ModuleManager()
{
}

bool ModuleManager::LoadModule(const std::string& aModuleName)
{
	auto it = mLoadedModules.find(aModuleName);
	if (it != mLoadedModules.end())
	{
		_LOG_CORE_WARNING("ModuleManager: Module already loaded: {}", aModuleName);
		return false;
	}

	HMODULE lib = LoadLibraryA((aModuleName + "_" + BUILD_CONFIG + ".dll").c_str());
	if (!lib)
	{
		_LOG_CORE_WARNING("ModuleManager: Module failed to load (LoadLibraryA): {}", aModuleName);
		return false;
	}

	auto createFunction = (IModule * (*)())GetProcAddress(lib, "CreateModule");
	if (!createFunction)
	{
		_LOG_CORE_WARNING("ModuleManager: Create function did not exist: {}", aModuleName);
		return false;
	}

	IModule* module = createFunction();
	if (!module)
	{
		_LOG_CORE_WARNING("ModuleManager: Module create function did not return vaild IModule: {}", aModuleName);
		return false;
	}

	module->Startup();

	ModuleInfo info;
	info.library = lib;
	info.module = module;

	mLoadedModules.insert({ aModuleName, info });

	_LOG_CORE_INFO("ModuleManager: Module load success: {}", aModuleName);
	return true;
}

bool ModuleManager::UnLoadModule(const std::string& aModuleName)
{
	auto it = mLoadedModules.find(aModuleName);
	if (it == mLoadedModules.end())
	{
		_LOG_CORE_WARNING("ModuleManager: Unload Module did not exist: {}", aModuleName);
		return false;
	}

	it->second.module->Shutdown();
	FreeLibrary(it->second.library);
	mLoadedModules.erase(aModuleName);

	_LOG_CORE_INFO("ModuleManager: Module unload success: {}", aModuleName);
	return true;
}

const IModule* ModuleManager::GetModule(const std::string& aModuleName) const
{
	auto it = mLoadedModules.find(aModuleName);
	if (it == mLoadedModules.end())
		return nullptr;

	return it->second.module;
}

HMODULE ModuleManager::GetHModule(const std::string& aModuleName) const
{
	auto it = mLoadedModules.find(aModuleName);
	if (it == mLoadedModules.end())
		return HMODULE();

	return it->second.library;
}
