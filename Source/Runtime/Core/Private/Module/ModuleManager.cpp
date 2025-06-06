#include "pch.h"
#include "Module/ModuleManager.h"

ModuleManager::ModuleManager()
{
}

ModuleManager::~ModuleManager()
{
}

bool ModuleManager::LoadModule(const String& ModuleName)
{
	auto it = mLoadedModules.find(ModuleName);
	if (it != mLoadedModules.end())
	{
		_LOG_CORE_WARNING("ModuleManager: Module already loaded: {}", ModuleName);
		return false;
	}

	HMODULE lib = LoadLibraryA((ModuleName + "_" + BUILD_CONFIG + ".dll").c_str());
	if (!lib)
	{
		_LOG_CORE_WARNING("ModuleManager: Module failed to load (LoadLibraryA): {}", ModuleName);
		return false;
	}

	auto createFunction = (IModule * (*)())GetProcAddress(lib, "CreateModule");
	if (!createFunction)
	{
		_LOG_CORE_WARNING("ModuleManager: Create function did not exist: {}", ModuleName);
		return false;
	}

	IModule* module = createFunction();
	if (!module)
	{
		_LOG_CORE_WARNING("ModuleManager: Module create function did not return vaild IModule: {}", ModuleName);
		return false;
	}

	module->Startup();

	ModuleInfo info;
	info.library = lib;
	info.module = module;

	mLoadedModules.insert({ ModuleName, info });

	_LOG_CORE_INFO("ModuleManager: Module load success: {}", ModuleName);
	return true;
}

bool ModuleManager::UnLoadModule(const String& ModuleName)
{
	auto it = mLoadedModules.find(ModuleName);
	if (it == mLoadedModules.end())
	{
		_LOG_CORE_WARNING("ModuleManager: Unload Module did not exist: {}", ModuleName);
		return false;
	}

	it->second.module->Shutdown();
	FreeLibrary(it->second.library);
	mLoadedModules.erase(ModuleName);

	_LOG_CORE_INFO("ModuleManager: Module unload success: {}", ModuleName);
	return true;
}

const IModule* ModuleManager::GetModule(const String& ModuleName) const
{
	auto it = mLoadedModules.find(ModuleName);
	if (it == mLoadedModules.end())
		return nullptr;

	return it->second.module;
}

HMODULE ModuleManager::GetHModule(const String& ModuleName) const
{
	auto it = mLoadedModules.find(ModuleName);
	if (it == mLoadedModules.end())
		return HMODULE();

	return it->second.library;
}
