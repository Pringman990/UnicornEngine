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

void ReflectionRegistry::RegisterType(ReflectionTypeInfo aInfo)
{
	_LOG_CORE_INFO("Reflection registry type registered: {}", aInfo.name);

	auto it = mNameToType.find(aInfo.name);
	if (it != mNameToType.end())
	{
		_LOG_CORE_CRITICAL("ReflectionRegistry: Trying to register already registered: {}", aInfo.name);
		return;
	}

	mRegistry[*aInfo.typeIndex] = aInfo;
	mNameToType[aInfo.name] = aInfo;
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(std::type_index aType)
{
	auto it = mRegistry.find(aType);
	if (it != mRegistry.end())
		return mRegistry.at(aType);

	return ReflectionTypeInfo();
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(const String& aName)
{
	auto it = mNameToType.find(aName);
	if (it != mNameToType.end())
		return it->second;

	return ReflectionTypeInfo();
}

void ReflectionRegistry::EnqueueDefferedRegistration(DeffRegistartionFn aRegFunction)
{
	GetDefferedQueue().push_back(aRegFunction);
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