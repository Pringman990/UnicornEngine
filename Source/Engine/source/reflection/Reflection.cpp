#include "EnginePch.h"
#include "Reflection.h"
#include <functional>

reflection::Registry::Registry()
{
}

reflection::Registry::~Registry()
{
}

void reflection::Registry::ShutDown()
{
	mRegistry.clear();
	mNameToType.clear();
}

void reflection::Registry::RegisterType(std::type_index aType, TypeInfo aInfo)
{
	mRegistry[aType] = aInfo;
	mNameToType[aInfo.name] = aInfo;
}

reflection::TypeInfo reflection::Registry::GetTypeInfo(std::type_index aType)
{
	auto it = mRegistry.find(aType);
	if(it != mRegistry.end())
		return mRegistry.at(aType);

	return TypeInfo();
}

reflection::TypeInfo reflection::Registry::GetTypeInfo(const std::string& aName)
{
	auto it = mNameToType.find(aName);
	if(it != mNameToType.end())
		return it->second;

	return TypeInfo();
}
