#include "Reflection/ReflectionRegistry.h"

ReflectionRegistry::ReflectionRegistry()
{
}

ReflectionRegistry::~ReflectionRegistry()
{
}

void ReflectionRegistry::RegisterType(const ReflectionRegistry::TypeInfo& Info)
{
	auto it = mData.find(Info.type);
	if (it != mData.end())
	{
		_LOG_CORE_CRITICAL("Trying to register already registered reflected type: {}", Info.name);
		return;
	}

	mData[Info.type] = Info;
}

const ReflectionRegistry::TypeInfo& ReflectionRegistry::GetInfo(TypeIdHash Hash)
{
	return mData[Hash];
}

Vector<ReflectionRegistry::TypeInfo> ReflectionRegistry::GetAllInfos()
{
	Vector<ReflectionRegistry::TypeInfo> infos;
	for (const auto& [hash, info] : mData)
	{
		infos.push_back(info);
	}

	return infos;
}
