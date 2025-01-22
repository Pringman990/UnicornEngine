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

void ReflectionRegistry::RegisterType(std::type_index aType, ReflectionTypeInfo aInfo)
{
	mRegistry[aType] = aInfo;
	mNameToType[aInfo.name] = aInfo;
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(std::type_index aType)
{
	auto it = mRegistry.find(aType);
	if (it != mRegistry.end())
		return mRegistry.at(aType);

	return ReflectionTypeInfo();
}

ReflectionTypeInfo ReflectionRegistry::GetTypeInfo(const std::string& aName)
{
	auto it = mNameToType.find(aName);
	if (it != mNameToType.end())
		return it->second;

	return ReflectionTypeInfo();
}
