 #include "Module/ModuleManager.h"

REGISTER_ENGINE_SUBSYSTEM(ModuleManager)

MultiNotifierArgs<String> ModuleManager::OnModuleUnload;

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
		LOG_WARNING("ModuleManager: Module already loaded: {}", ModuleName);
		return false;
	}

	HMODULE lib = LoadLibraryA((ModuleName + "_" + BUILD_CONFIG + ".dll").c_str());
	if (!lib)
	{
		LOG_WARNING("ModuleManager: Module failed to load (LoadLibraryA): {}", ModuleName);
		return false;
	}

	auto createFunction = (IModule * (*)())GetProcAddress(lib, "CreateModule");
	if (!createFunction)
	{
		LOG_WARNING("ModuleManager: Create function did not exist: {}", ModuleName);
		return false;
	}

	IModule* module = createFunction();
	if (!module)
	{
		LOG_WARNING("ModuleManager: Module create function did not return vaild IModule: {}", ModuleName);
		return false;
	}

	module->Startup();

	ModuleInfo info;
	info.library = lib;
	info.module = module;
	info.name = ModuleName;

	mLoadedModules.insert({ ModuleName, info });

	LOG_INFO("ModuleManager: Module load success: {}", ModuleName);
	return true;
}

bool ModuleManager::UnLoadModule(const String& ModuleName)
{
	auto it = mLoadedModules.find(ModuleName);
	if (it == mLoadedModules.end())
	{
		LOG_WARNING("ModuleManager: Unload Module did not exist: {}", ModuleName);
		return false;
	}

	OnModuleUnload.Notify(ModuleName);

	it->second.module->Shutdown();
	FreeLibrary(it->second.library);
	mLoadedModules.erase(ModuleName);

	LOG_INFO("ModuleManager: Module unload success: {}", ModuleName);
	return true;
}

const ModuleInfo* ModuleManager::TryGetModule(const String& ModuleName) const
{
	auto it = mLoadedModules.find(ModuleName);
	if (it == mLoadedModules.end())
		return nullptr;

	return &it->second;
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
