#include "pch.h"
#include "Reflection/ReflectionRegistry.h"

ReflectionRegistry::ReflectionRegistry()
{
}

ReflectionRegistry::~ReflectionRegistry()
{
	mRegistry.clear();
	mNameToType.clear();
}

void ReflectionRegistry::RegisterType(ReflectionTypeInfo Info)
{
	_LOG_CORE_INFO("Reflection registry type registered: {}", Info.name);

	auto it = mNameToType.find(Info.name);
	if (it != mNameToType.end())
	{
		_LOG_CORE_CRITICAL("ReflectionRegistry: Trying to register already registered: {}", Info.name);
		return;
	}

	mRegistry[*Info.typeIndex] = Info;
	mNameToType[Info.name] = Info;
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(std::type_index Type)
{
	auto it = mRegistry.find(Type);
	if (it != mRegistry.end())
		return mRegistry.at(Type);

	return ReflectionTypeInfo();
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(const String& Name)
{
	auto it = mNameToType.find(Name);
	if (it != mNameToType.end())
		return it->second;

	return ReflectionTypeInfo();
}

void ReflectionRegistry::EnqueueDefferedRegistration(DeffRegistartionFn RegFunction)
{
	GetDefferedQueue().push_back(RegFunction);
}

void ReflectionRegistry::ProcessDefferedRegistration()
{
#ifdef _DEBUG_PRINT_REFLECTION_REGISTRY
		_LOG_CORE_INFO("================================================");
#endif // _DEBUG_PRINT_REFLECTION_REGISTRY


	_LOG_CORE_INFO("Processing deffered reflection registry...");
	for (auto& fn : GetDefferedQueue())
	{
		fn();
	}
	GetDefferedQueue().clear();
	_LOG_CORE_INFO("Processing deffered reflection registry complete");

#ifdef _DEBUG_PRINT_REFLECTION_REGISTRY
	_LOG_CORE_INFO("================================================");
#endif // _DEBUG_PRINT_REFLECTION_REGISTRY
}

Vector<ReflectionRegistry::DeffRegistartionFn>& ReflectionRegistry::GetDefferedQueue()
{
	static Vector<DeffRegistartionFn> queue;
	return queue;
}