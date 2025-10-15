#include "Reflection/ReflectionRegistry.h"

namespace
{
	ENGINE_API UnorderedMap<UniqueID128, Reflect::ReflectTypeInfo> __sReflectionData;
}

namespace __Reflection
{
	void RegisterType(const Reflect::TypeSchemaCompile* Schema)
	{
		Reflect::ReflectTypeInfo typeInfo{};
		typeInfo.schema = Reflect::TypeSchema::CopyCompileToRuntime(Schema);
		typeInfo.loaded = true;

		auto it = __sReflectionData.find(Schema->uuid);
		if (it != __sReflectionData.end())
		{
			it->second = typeInfo;
			auto* t = &it->second;

			REFLECTION_LOG_TYPE_INFO(t);

			return;
		}

		__sReflectionData[Schema->uuid] = typeInfo;
	}

	void NullifyType(UniqueID128 ID)
	{
		auto it = __sReflectionData.find(ID);
		if (it != __sReflectionData.end())
		{
			Reflect::TypeSchema::Invalidate(it->second.schema);
			LOG_INFO("Type Nulled: {}", it->second.schema.name);
		}
		LOG_WARNING("Tried to null non registered type: {}", ID.ToString().c_str());
	}
	
	Vector<const Reflect::ReflectTypeInfo*> GetAllTypes()
	{
		Vector<const Reflect::ReflectTypeInfo*> infos;
		for (const auto& [hash, info] : __sReflectionData)
		{
			infos.push_back(&info);
		}

		return infos;
	}

	const Reflect::ReflectTypeInfo* TryGetInfo(UniqueID128 ID)
	{
		auto it = __sReflectionData.find(ID);
		if (it == __sReflectionData.end())
		{
			LOG_WARNING("Trying to get a non registered ReflectTypeInfo");
			return nullptr;
		}

		return &it->second;
	}

	template<typename T>
	const Reflect::ReflectTypeInfo* TryGetInfo()
	{
		UniqueID128 uuid = GetUUID<T>();
		auto it = __sReflectionData.find(uuid);
		return it != __sReflectionData.end() ? &it->second : nullptr;
	}
}