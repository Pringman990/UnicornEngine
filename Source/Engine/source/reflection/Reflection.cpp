#include "EnginePch.h"
#include "Reflection.h"

void reflection::Registry::ShutDown()
{
	mRegistry.clear();
}

void reflection::Registry::RegisterType(std::type_index aType, TypeInfo aInfo)
{
	mRegistry.insert({aType, aInfo});
}

reflection::TypeInfo reflection::Registry::GetTypeInfo(std::type_index aType)
{
	auto it = mRegistry.find(aType);
	if(it != mRegistry.end())
		return mRegistry.at(aType);

	return TypeInfo();
}
